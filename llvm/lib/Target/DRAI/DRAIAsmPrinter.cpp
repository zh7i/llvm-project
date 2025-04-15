//===-- DRAIAsmPrinter.cpp - DRAI LLVM assembly writer ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format DRAI assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/DRAIInstPrinter.h"
//#include "MCTargetDesc/DRAIMCExpr.h"
//#include "MCTargetDesc/DRAITargetStreamer.h"
#include "DRAI.h"
#include "DRAIInstrInfo.h"
#include "DRAIMCInstLower.h"
#include "DRAITargetMachine.h"
#include "TargetInfo/DRAITargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// TODO:
// %hi16() and %lo16() for addresses

namespace llvm {
class DRAIAsmPrinter : public AsmPrinter {
#if 0
    DRAITargetStreamer &getTargetStreamer() {
      return static_cast<DRAITargetStreamer &>(
          *OutStreamer->getTargetStreamer());
    }
#endif
public:
  explicit DRAIAsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "DRAI Assembly Printer"; }

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
  void emitInstruction(const MachineInstr *MI) override;

  void emitFunctionBodyStart() override;
};
} // end of anonymous namespace

bool DRAIAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                     const char *ExtraCode, raw_ostream &OS) {
  if (ExtraCode)
    return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, OS);
  DRAIMCInstLower Lower(MF->getContext(), *this);
  MCOperand MO(Lower.lowerOperand(MI->getOperand(OpNo)));
  DRAIInstPrinter::printOperand(MO, MAI, OS);
  return false;
}

void DRAIAsmPrinter::emitInstruction(const MachineInstr *MI) {
  MCInst LoweredMI;
  switch (MI->getOpcode()) {
  default:
    DRAIMCInstLower Lower(MF->getContext(), *this);
    Lower.lower(MI, LoweredMI);
    // doLowerInstr(MI, LoweredMI);
    break;
  }
  EmitToStreamer(*OutStreamer, LoweredMI);
}

void DRAIAsmPrinter::emitFunctionBodyStart() {
  llvm::dbgs() << "emitFunctionBodyStart!!!\n";

  // emit move address to %arg
  if (OutStreamer->hasRawTextSupport()) {
    OutStreamer->emitRawText(StringRef("\tmove.ei %arg, 0"));
  } else {
    SmallVector<MCInst, 2> MCInsts(1);
    MCOperand Arg = MCOperand::createReg(DRAI::ArgBase);
    MCOperand Def = MCOperand::createImm(0);

    // move.ei %arg, 0
    MCInsts[0].setOpcode(DRAI::MoveI_move_ei);
    MCInsts[0].addOperand(Arg);
    MCInsts[0].addOperand(Def);

    for (auto I : MCInsts) {
      OutStreamer->emitInstruction(I, getSubtargetInfo());
    }
  }
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDRAIAsmPrinter() {
  RegisterAsmPrinter<DRAIAsmPrinter> X(getTheDRAITarget());
}
