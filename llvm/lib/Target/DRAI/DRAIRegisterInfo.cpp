//===-- DRAIRegisterInfo.cpp - DRAI Register Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the DRAI implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "DRAIRegisterInfo.h"
#include "DRAI.h"
//#include "DRAIMachineFunctionInfo.h"
#include "DRAISubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "DRAIGenRegisterInfo.inc"

DRAIRegisterInfo::DRAIRegisterInfo() : DRAIGenRegisterInfo(DRAI::PseudoR) {}

const MCPhysReg *
DRAIRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_DRAI_SaveList;
}

BitVector DRAIRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // is the stack pointer.
  Reserved.set(DRAI::PseudoR);

  return Reserved;
}

bool DRAIRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                           int SPAdj, unsigned FIOperandNum,
                                           RegScavenger *RS) const {
  return false;
}

Register DRAIRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return DRAI::PseudoR;
}
