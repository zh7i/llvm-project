//===-- DRAIFrameLowering.cpp - Frame lowering for DRAI -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DRAIFrameLowering.h"
//#include "DRAICallingConv.h"
//#include "DRAIInstrBuilder.h"
//#include "DRAIInstrInfo.h"
//#include "DRAIMachineFunctionInfo.h"
#include "DRAIRegisterInfo.h"
#include "DRAISubtarget.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

DRAIFrameLowering::DRAIFrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(8), 0,
                          Align(8), false /* StackRealignable */),
      RegSpillOffsets(0) {}

void DRAIFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {}

void DRAIFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {}

bool DRAIFrameLowering::hasFP(const MachineFunction &MF) const { return false; }
