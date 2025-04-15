//===-- DRAIMCAsmBackend.cpp - DRAI assembler backend ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//#include "MCTargetDesc/DRAIMCFixups.h"
#include "MCTargetDesc/DRAIMCTargetDesc.h"
#include "MCTargetDesc/DRAIFixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

namespace {
class DRAIMCAsmBackend : public MCAsmBackend {
  uint8_t OSABI;

public:
  DRAIMCAsmBackend(uint8_t osABI) : MCAsmBackend(support::little), OSABI(osABI) {}

  // Override MCAsmBackend
  unsigned getNumFixupKinds() const override {
    return DRAI::NumTargetFixupKinds;
  }
  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;
  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;
  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *Fragment,
                            const MCAsmLayout &Layout) const override {
    return false;
  }
  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createDRAIObjectWriter(OSABI);
  }
};
} // end anonymous namespace

const MCFixupKindInfo &
DRAIMCAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[DRAI::NumTargetFixupKinds] = {
    // This table *must* be in same the order of fixup_* kinds in
    // DRAIFixupKinds.h
    //
    // name                        offset     bits    flags
    { "FixupRelPC32",              32,        32,     MCFixupKindInfo::FKF_IsPCRel /* same section relative offset */ },
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext *Ctx = nullptr) {
  unsigned Kind = Fixup.getKind();

  switch(Kind) {
  default:
    llvm_unreachable("unsupport fixup kind");
    break;
  case DRAI::FixupRelPC32:
    int64_t sval = Value;
    if (sval < std::numeric_limits<int32_t>::min() ||
        sval > std::numeric_limits<int32_t>::max()) {
      llvm_unreachable("fixup error, value exceed range");
    }
    break;
  }

  return Value;
}

void DRAIMCAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                  const MCValue &Target,
                                  MutableArrayRef<char> Data, uint64_t Value,
                                  bool IsResolved,
                                  const MCSubtargetInfo *STI) const {
  MCFixupKind Kind = Fixup.getKind();
  Value = adjustFixupValue(Fixup, Value);

  MCFixupKindInfo Info = getFixupKindInfo(Kind);
  assert(Info.TargetSize % 8 == 0 && Info.TargetOffset % 8 == 0 &&
         "bits num must be 8 align");

  unsigned TargetOffset = Info.TargetOffset / 8;
  unsigned NumBytes = Info.TargetSize / 8;

  uint32_t Offset = Fixup.getOffset() + TargetOffset;
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the bits from
  // the fixup value.
  for (unsigned i = 0; i != NumBytes; ++i)
    Data[Offset + i] |= static_cast<uint8_t>((Value >> (i * 8)) & 0xff);
}

bool DRAIMCAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                  const MCSubtargetInfo *STI) const {
  // Cannot emit NOP with size being not multiple of 16 bits.
  if (Count % 2 != 0)
    return false;

  uint64_t NumNops = Count / 2;
  for (uint64_t i = 0; i != NumNops; ++i) {
    OS << "\x4E\x71";
  }

  return true;
}

MCAsmBackend *llvm::createDRAIMCAsmBackend(const Target &T,
                                           const MCSubtargetInfo &STI,
                                           const MCRegisterInfo &MRI,
                                           const MCTargetOptions &Options) {
  uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new DRAIMCAsmBackend(OSABI);
}
