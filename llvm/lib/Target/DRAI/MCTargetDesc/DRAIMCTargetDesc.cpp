//===-- DRAIMCTargetDesc.cpp - DRAI target descriptions -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DRAIMCTargetDesc.h"
#include "DRAIInstPrinter.h"
#include "DRAIMCAsmInfo.h"
#include "TargetInfo/DRAITargetInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include <iostream>

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "DRAIGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "DRAIGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "DRAIGenRegisterInfo.inc"

static MCAsmInfo *createDRAIMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT,
                                      const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new DRAIMCAsmInfo(TT);
  return MAI;
}

static MCInstrInfo *createDRAIMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitDRAIMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createDRAIMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitDRAIMCRegisterInfo(X, llvm::DRAI::PseudoR, 0, 0, llvm::DRAI::PseudoR);
  return X;
}

static MCSubtargetInfo *createDRAIMCSubtargetInfo(const Triple &TT,
                                                  StringRef CPU, StringRef FS) {
  return createDRAIMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCInstPrinter *createDRAIMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new DRAIInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDRAITargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(getTheDRAITarget(), createDRAIMCAsmInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheDRAITarget(),
                                        createDRAIMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(getTheDRAITarget(),
                                      createDRAIMCInstrInfo);
  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(getTheDRAITarget(),
                                    createDRAIMCRegisterInfo);

  // Register the MCSubtargetInfo.
  TargetRegistry::RegisterMCSubtargetInfo(getTheDRAITarget(),
                                          createDRAIMCSubtargetInfo);
  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(getTheDRAITarget(),
                                       createDRAIMCAsmBackend);
  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheDRAITarget(),
                                        createDRAIMCInstPrinter);
}
