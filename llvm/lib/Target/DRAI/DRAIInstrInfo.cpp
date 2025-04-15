//===-- DRAIInstrInfo.cpp - DRAI instruction information ------------------===//
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

#include "DRAIInstrInfo.h"
#include "DRAI.h"
#include "MCTargetDesc/DRAIMCTargetDesc.h"
//#include "DRAIInstrBuilder.h"
#include "DRAISubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveVariables.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/BranchProbability.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Target/TargetMachine.h"
#include <cassert>
#include <cstdint>
#include <iterator>

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRMAP_INFO
#include "DRAIGenInstrInfo.inc"

#define DEBUG_TYPE "drai-ii"

// Pin the vtable to this file.
void DRAIInstrInfo::anchor() {}

DRAIInstrInfo::DRAIInstrInfo(DRAISubtarget &STI)
    : DRAIGenInstrInfo(), RI(), STI(STI) {}

void DRAIInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I,
                                const DebugLoc &DL, MCRegister DestReg,
                                MCRegister SrcReg, bool KillSrc) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  if (DRAI::SRegRegClass.contains(DestReg) && DRAI::SRegRegClass.contains(SrcReg)) {
    BuildMI(MBB, I, DL, get(DRAI::MoveS_move_ss), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  if (DRAI::ERegRegClass.contains(DestReg) && DRAI::ERegRegClass.contains(SrcReg)) {
    BuildMI(MBB, I, DL, get(DRAI::MoveS_move_ee), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  if (DRAI::VRegRegClass.contains(DestReg) && DRAI::VRegRegClass.contains(SrcReg)) {
    Register undef = MRI.createVirtualRegister(&DRAI::VRegRegClass);
    BuildMI(MBB, I, DL, get(DRAI::MoveV_move_vv), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc)).addReg(DRAI::PredTruth).addReg(undef);
    return;
  }
  if (DRAI::PRegRegClass.contains(DestReg) && DRAI::PRegRegClass.contains(SrcReg)) {
    Register undef = MRI.createVirtualRegister(&DRAI::PRegRegClass);
    BuildMI(MBB, I, DL, get(DRAI::MoveV_move_pp), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc)).addReg(DRAI::PredTruth).addReg(undef);
    return;
  }

  llvm_unreachable("Impossible reg-to-reg copy");
}

static std::vector<MachineInstr *> getTerminatorInstrs(MachineBasicBlock *mbb) {
  std::vector<MachineInstr *> ters;
  for (MachineInstr &inst : mbb->terminators()) {
    ters.push_back(&inst);
  }
  return ters;
}

bool DRAIInstrInfo::expandLoopPseudo(MachineInstr &MI) const {
  if (MI.getOpcode() == DRAI::LoopPseudoIterDec) {
    return false; // replaced by LoopPseudoSetIter
  }

  MachineInstr *set_iter = &MI;
  uint32_t loop_id = set_iter->getOperand(0).getImm();

  // find iter_dec
  MachineFunction *MF = MI.getParent()->getParent();
  MachineInstr *iter_dec = nullptr;
  for (MachineBasicBlock &block : *MF) {
    for (MachineInstr &inst : block) {
      if (inst.getOpcode() == DRAI::LoopPseudoIterDec) {
        MachineOperand &op_id = inst.getOperand(1); // 0 is ret value
        if ((uint32_t)op_id.getImm() == loop_id) {
          iter_dec = &inst;
        }
      }
    }
  }
  if (!iter_dec) {
    llvm_unreachable("drai.loop.iter.dec not found!!");
  }

  // find blocks & branchs
  MachineBasicBlock *preheader = set_iter->getParent();
  std::vector<MachineInstr *> pre_ters = getTerminatorInstrs(preheader);
  if (pre_ters.at(0) != set_iter) {
    llvm_unreachable("unexpect preheader first terminator!!");
  }
  [[maybe_unused]] MachineBasicBlock *header = nullptr;
  if (pre_ters.size() > 1) {
    MachineInstr *pre_br = &(*pre_ters.at(1));
    if (!pre_br || pre_br->getOpcode() != DRAI::JmpFirst ||
        pre_br->getOperand(0).getReg() != DRAI::PredTruth) {
      llvm_unreachable("unexpect preheader branch!!");
    }
    header = pre_br->getOperand(1).getMBB();
  } else { // optimized in SelectionDAGBuilder::visitBr
    MachineFunction::iterator I(preheader);
    if (++I == MF->end()) {
      llvm_unreachable("unexpect preheader's next mbb!!");
    }
    header = &(*I);
  }
  MachineBasicBlock *latch = iter_dec->getParent();
  std::vector<MachineInstr *> latch_ters = getTerminatorInstrs(latch);
  MachineInstr *lp_condbr = &(*latch_ters.at(0));
  MachineInstr *lp_br = &(*latch_ters.at(1));
  if (lp_condbr == iter_dec) {
    lp_condbr = &(*latch_ters.at(1));
    lp_br = &(*latch_ters.at(2));
  }
  if (!lp_condbr || !lp_br) {
    llvm_unreachable("unexpect latch branch!!");
  }
  MachineBasicBlock *lp_end = lp_br->getOperand(1).getMBB();

  // get depth & loop count
  int64_t depth = set_iter->getOperand(1).getImm();
  Register lp_count = set_iter->getOperand(2).getReg();
  if (depth < 0 || depth >= 255) {
    llvm_unreachable("loop depth exceed i8 value range!!");
  }

  // create loop instruction
  MachineBasicBlock::iterator position = set_iter; // before this
  BuildMI(*preheader, position, position->getDebugLoc(), get(DRAI::Loop))
      .addImm(depth)
      .addReg(lp_count)
      .addMBB(lp_end);

  // remove pseudo & cond branch
  set_iter->eraseFromParent();
  lp_condbr->eraseFromParent();
  iter_dec->eraseFromParent();

  return true;
}

bool DRAIInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  dbgs() << "expandPostRAPseudo: ";
  MI.dump();

  switch (MI.getOpcode()) {
  default:
    break;
  case DRAI::LoopPseudoSetIter:
  case DRAI::LoopPseudoIterDec:
    return expandLoopPseudo(MI);
  }

  return false;
}