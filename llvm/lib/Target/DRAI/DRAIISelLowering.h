//===-- DRAIISelLowering.h - DRAI DAG lowering interface --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that DRAI uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DRAI_DRAIISELLOWERING_H
#define LLVM_LIB_TARGET_DRAI_DRAIISELLOWERING_H

#include "DRAI.h"
#include "DRAIInstrInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class DRAISubtarget;
class DRAISubtarget;

namespace DRAIISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  // // Return with a flag operand.  Operand 0 is the chain operand.
  RET_FLAG,

  // // Calls a function.  Operand 0 is the chain operand and operand 1
  // // is the target address.  The arguments start at operand 2.
  // // There is an optional glue operand at the end.
  // CALL,

  // // Bit-field instructions.
  // CLR,
  // SET,
  // EXT,
  // EXTU,
  // MAK,
  // ROT,
  // FF1,
  // FF0,

  // ConstantFP,

  JmpAll,
  JmpAny,
  JmpNone,

  Loop,
};
} // end namespace DRAIISD

class DRAITargetLowering : public TargetLowering {
  const DRAISubtarget &Subtarget;

  // SDValue LowerEXTRACT_VECTOR_ELT(SDValue Op, SelectionDAG &DAG) const;

  // SDValue LowerConstantFP(SDValue Op, SelectionDAG &DAG) const;

  // SDValue LowerLOAD(SDValue Op, SelectionDAG &DAG) const;
  // SDValue LowerSTORE(SDValue Op, SelectionDAG &DAG) const;
  // SDValue LowerSIGN_EXTEND(SDValue Op, SelectionDAG &DAG) const;
  // SDValue LowerMGATHER(SDValue Op, SelectionDAG &DAG) const;
  // SDValue LowerMSCATTER(SDValue Op, SelectionDAG &DAG) const;
  // SDValue LowerSHL(SDValue Op, SelectionDAG &DAG) const;

public:
  explicit DRAITargetLowering(const TargetMachine &TM,
                              const DRAISubtarget &STI);

  // Override TargetLowering methods.
  bool hasAndNot(SDValue X) const override { return true; }
  const char *getTargetNodeName(unsigned Opcode) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;

  // Override required hooks.
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;

  EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                         EVT VT) const override;

  MVT getScalarShiftAmountTy(const DataLayout &DL, EVT LHSTy) const override;

  TargetLoweringBase::LegalizeTypeAction
  getPreferredVectorAction(MVT VT) const override;

  SDValue BuildSDIVPow2(SDNode *N, const APInt &Divisor, SelectionDAG &DAG,
                        SmallVectorImpl<SDNode *> &Created) const override {
    // avoid generic combine to sra
    return SDValue(N, 0); // keep
  }

  bool getTgtMemIntrinsic(IntrinsicInfo &Info, const CallInst &I,
                          MachineFunction &MF, unsigned Intrinsic) const override;

};

} // end namespace llvm

#endif
