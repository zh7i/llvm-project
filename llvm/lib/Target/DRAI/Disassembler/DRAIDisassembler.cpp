//===-- DRAIDisassembler.cpp - Disassembler for DRAI ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is part of the DRAI Disassembler.
//
//===----------------------------------------------------------------------===//

#include "DRAI.h"
#include "DRAIRegisterInfo.h"
#include "DRAISubtarget.h"
//#include "MCTargetDesc/DRAIMCCodeEmitter.h"
#include "MCTargetDesc/DRAIMCTargetDesc.h"
#include "TargetInfo/DRAITargetInfo.h"

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "drai-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

static std::optional<unsigned> getClassReg(const MCDisassembler *D, unsigned RC,
                                           unsigned RegNo) {
  const MCRegisterInfo *RegInfo = D->getContext().getRegisterInfo();
  const MCRegisterClass &RegClass = RegInfo->getRegClass(RC);
  if (RegNo >= RegClass.getNumRegs()) {
    return std::nullopt;
  }
  return *(RegClass.begin() + RegNo);
}

static DecodeStatus DecodeSRegRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  if (auto Reg = getClassReg(Decoder, DRAI::SRegRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(Reg.value()));
    return DecodeStatus::Success;
  }
  return DecodeStatus::Fail;
}

static DecodeStatus DecodeERegRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  if (auto Reg = getClassReg(Decoder, DRAI::ERegRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(Reg.value()));
    return DecodeStatus::Success;
  }
  return DecodeStatus::Fail;
}

static DecodeStatus DecodeVRegRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  if (auto Reg = getClassReg(Decoder, DRAI::VRegRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(Reg.value()));
    return DecodeStatus::Success;
  }
  return DecodeStatus::Fail;
}

static DecodeStatus DecodePRegRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  if (auto Reg = getClassReg(Decoder, DRAI::PRegRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(Reg.value()));
    return DecodeStatus::Success;
  }
  return DecodeStatus::Fail;
}

#include "DRAIGenDisassemblerTables.inc"

/// A disassembler class for DRAI.
struct DRAIDisassembler : public MCDisassembler {
  DRAIDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  virtual ~DRAIDisassembler() {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};

DecodeStatus DRAIDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                              ArrayRef<uint8_t> Bytes,
                                              uint64_t Address,
                                              raw_ostream &CStream) const {
  DecodeStatus Result;

  // Result = readInstruction64(Bytes, Address, Size, Insn);
  // if(Result == MCDisassembler::Fail){
  //   return MCDisassembler::Fail;
  // }

  //auto MakeUp = [&](APInt &Insn, unsigned InstrBits) {
  //  unsigned Idx = Insn.getBitWidth() >> 3;
  //  unsigned RoundUp = alignTo(InstrBits, Align(16));
  //  if (RoundUp > Insn.getBitWidth())
  //    Insn = Insn.zext(RoundUp);
  //  RoundUp = RoundUp >> 3;
  //  for (; Idx < RoundUp; Idx += 2) {
  //    Insn.insertBits(support::endian::read16be(&Bytes[Idx]), Idx * 8, 16);
  //  }
  //};
  APInt Insn(64, support::endian::read64le(Bytes.data()));
  // 2 bytes of data are consumed, so set Size to 2
  // If we don't do this, disassembler may generate result even
  // the encoding is invalid. We need to let it fail correctly.
  Size = 8;
  Result = decodeInstruction(DecoderTableDRAI64, Instr, Insn, Address, this, STI);
  //if (Result == DecodeStatus::Success)
  //  Size = InstrLenTable[Instr.getOpcode()] >> 3;
  return Result;
}

static MCDisassembler *createDRAIDisassembler(const Target &T,
                                              const MCSubtargetInfo &STI,
                                              MCContext &Ctx) {
  return new DRAIDisassembler(STI, Ctx);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDRAIDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(getTheDRAITarget(),
                                         createDRAIDisassembler);
}
