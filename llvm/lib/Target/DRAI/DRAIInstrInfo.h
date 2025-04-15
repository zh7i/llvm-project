//===-- DRAIInstrInfo.h - DRAI instruction information ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the DRAI implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DRAI_DRAIINSTRINFO_H
#define LLVM_LIB_TARGET_DRAI_DRAIINSTRINFO_H

#include "DRAI.h"
#include "DRAIRegisterInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include <cstdint>

#define GET_INSTRINFO_HEADER
#include "DRAIGenInstrInfo.inc"

namespace llvm {

class DRAISubtarget;

class DRAIInstrInfo : public DRAIGenInstrInfo {
  const DRAIRegisterInfo RI;
  DRAISubtarget &STI;
  // const DRAIRegisterInfo RegInfo;

  virtual void anchor();

public:
  explicit DRAIInstrInfo(DRAISubtarget &STI);

  // Return the DRAIRegisterInfo, which this class owns.
  const DRAIRegisterInfo &getRegisterInfo() const { return RI; }

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;


  MachineMemOperand *getMemOperand(MachineBasicBlock &MBB, int FI,
                                   MachineMemOperand::Flags Flags) const;

  // void loadRegFromStackSlot(MachineBasicBlock &MBB,
  //                           MachineBasicBlock::iterator MI, Register DestReg,
  //                           int FrameIndex, const TargetRegisterClass *RC,
  //                           const TargetRegisterInfo *TRI) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI, Register VReg) const override {
    MI->dump();
    llvm_unreachable("not support storeRegToStackSlot");
  }

 private:
  bool expandLoopPseudo(MachineInstr &MI) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_DRAI_DRAIINSTRINFO_H
