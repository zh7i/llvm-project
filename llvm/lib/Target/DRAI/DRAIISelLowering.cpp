//===-- DRAIISelLowering.cpp - DRAI DAG lowering implementation -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the DRAITargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "DRAIISelLowering.h"
#include "DRAICondCode.h"
//#include "DRAICallingConv.h"
//#include "DRAIConstantPoolValue.h"
//#include "DRAIMachineFunctionInfo.h"
#include "DRAITargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsDRAI.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/KnownBits.h"
#include <cctype>

using namespace llvm;

#define DEBUG_TYPE "drai-lower"

DRAITargetLowering::DRAITargetLowering(const TargetMachine &TM,
                                       const DRAISubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  addRegisterClass(MVT::i32, &DRAI::SRegRegClass);
  addRegisterClass(MVT::i64, &DRAI::ERegRegClass);
  addRegisterClass(MVT::f16, &DRAI::SRegRegClass);
  addRegisterClass(MVT::f32, &DRAI::SRegRegClass);
  addRegisterClass(Subtarget.Vec(MVT::i32), &DRAI::VRegRegClass);
  addRegisterClass(Subtarget.Vec(MVT::f16), &DRAI::VRegRegClass);
  addRegisterClass(Subtarget.Vec(MVT::f32), &DRAI::VRegRegClass);
  addRegisterClass(MVT::i1, &DRAI::PRegRegClass);
  addRegisterClass(Subtarget.Vec(MVT::i1), &DRAI::PRegRegClass);

  // Compute derived properties from the register classes
  computeRegisterProperties(Subtarget.getRegisterInfo());

  // Set up special registers.
  setStackPointerRegisterToSaveRestore(DRAI::PseudoR);

  // How we extend i1 boolean values.
  setBooleanContents(ZeroOrOneBooleanContent);

  // NVPTX backend guarantee sync instruction order
  // setSchedulingPreference(Sched::Source);

  setMinFunctionAlignment(Align(8));
  setPrefFunctionAlignment(Align(8));

  // can not use MVT::Any
  setOperationAction(ISD::ConstantFP, MVT::f32, Legal); // avoid convert to ConstantPool
  setOperationAction(ISD::ConstantFP, MVT::f16, Legal);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand); // avoid setcc+jmp combine to this
  setOperationAction(ISD::BR_CC, MVT::i64, Expand); // integer mask
  setOperationAction(ISD::BR_CC, MVT::f32, Expand);
  setOperationAction(ISD::BR_CC, MVT::f16, Expand);
  setOperationAction(ISD::BR_CC, MVT::i1, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand); // avoid setcc+select combine to this
  setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f16, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i64, Expand);
  setOperationAction(ISD::SELECT_CC, Subtarget.Vec(MVT::i32), Expand);
  setOperationAction(ISD::SELECT_CC, Subtarget.Vec(MVT::f32), Expand);
  setOperationAction(ISD::SELECT_CC, Subtarget.Vec(MVT::f16), Expand);

  for (auto isd : {ISD::FSQRT, ISD::FSIN, ISD::FCOS, ISD::FEXP, ISD::FEXP2,
                   ISD::FLOG, ISD::FLOG10, ISD::FLOG2}) {
    setOperationAction(isd, MVT::f32, Legal);
    setOperationAction(isd, Subtarget.Vec(MVT::f32), Legal);
  }

  setOperationAction(ISD::VP_REDUCE_ADD, Subtarget.Vec(MVT::i32), Legal);
  setOperationAction(ISD::VP_REDUCE_FADD, Subtarget.Vec(MVT::f32), Legal);
  setOperationAction(ISD::VP_REDUCE_FADD, Subtarget.Vec(MVT::f16), Legal);
  setOperationAction(ISD::VP_REDUCE_SMAX, Subtarget.Vec(MVT::i32), Legal);
  setOperationAction(ISD::VP_REDUCE_FMAX, Subtarget.Vec(MVT::f32), Legal);
  setOperationAction(ISD::VP_REDUCE_FMAX, Subtarget.Vec(MVT::f16), Legal);
  setOperationAction(ISD::VP_REDUCE_SMIN, Subtarget.Vec(MVT::i32), Legal);
  setOperationAction(ISD::VP_REDUCE_FMIN, Subtarget.Vec(MVT::f32), Legal);
  setOperationAction(ISD::VP_REDUCE_FMIN, Subtarget.Vec(MVT::f16), Legal);
  // setOperationAction(ISD::VECREDUCE_ADD, Subtarget.Vec(MVT::i32), Legal);
  // setOperationAction(ISD::VECREDUCE_FADD, Subtarget.Vec(MVT::f32), Legal);
  // setOperationAction(ISD::VECREDUCE_FADD, Subtarget.Vec(MVT::f16), Legal);
  // setOperationAction(ISD::VECREDUCE_SMAX, Subtarget.Vec(MVT::i32), Legal);
  // setOperationAction(ISD::VECREDUCE_FMAX, Subtarget.Vec(MVT::f32), Legal);
  // setOperationAction(ISD::VECREDUCE_FMAX, Subtarget.Vec(MVT::f16), Legal);
  // setOperationAction(ISD::VECREDUCE_SMIN, Subtarget.Vec(MVT::i32), Legal);
  // setOperationAction(ISD::VECREDUCE_FMIN, Subtarget.Vec(MVT::f32), Legal);
  // setOperationAction(ISD::VECREDUCE_FMIN, Subtarget.Vec(MVT::f16), Legal);

  // temporary disable these, todo support
  setOperationAction(ISD::FNEG, MVT::f32, Expand);
  setOperationAction(ISD::FNEG, MVT::f16, Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Custom);

  setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MVT::i32, Expand); // avoid load+zext combine to this
  setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MVT::i32, Expand); // avoid load+sext combine to this
  setLoadExtAction(ISD::EXTLOAD, MVT::i64, MVT::i32, Expand); // avoid load+anyext combine to this

  setLoadExtAction(ISD::EXTLOAD, MVT::f32, MVT::f16, Expand); // avoid load+fp_extend combine to this
  setLoadExtAction(ISD::EXTLOAD, Subtarget.Vec(MVT::f32), Subtarget.Vec(MVT::f16), Expand);

  setTruncStoreAction(MVT::f32, MVT::f16, Expand); // avoid fp_round+store combine to this
  setTruncStoreAction(Subtarget.Vec(MVT::f32), Subtarget.Vec(MVT::f16), Expand);

  // setOperationAction(ISD::CTTZ, MVT::i32, Expand);
  // setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  // setOperationAction(ISD::VSELECT, Subtarget.Vec(MVT::i1), Expand);
  // setOperationAction(ISD::VSELECT, Subtarget.Vec(MVT::i32), Expand);
  // setOperationAction(ISD::EXTRACT_VECTOR_ELT, Subtarget.Vec(MVT::i32), Custom);

  // Special DAG combiner for bit-field operations.
  // setTargetDAGCombine(ISD::AND);
  // setTargetDAGCombine(ISD::OR);
  // setTargetDAGCombine(ISD::SHL);
  setTargetDAGCombine(ISD::BRCOND);
  setTargetDAGCombine(ISD::ZERO_EXTEND);
  setTargetDAGCombine(ISD::GlobalAddress);
  setTargetDAGCombine(ISD::INTRINSIC_W_CHAIN);

  // setHasMultipleConditionRegisters(true); // see llvm/lib/CodeGen/CodeGenPrepare.cpp: sinkCmpExpression()
}

// SDValue DRAITargetLowering::LowerEXTRACT_VECTOR_ELT(SDValue Op,
//                                                     SelectionDAG &DAG) const {
//   SDLoc DL(Op);
//   SDValue Vector = Op.getOperand(0);
//   SDValue Index = Op.getOperand(1);

//   // if (isa<ConstantSDNode>(Index) ||
//   //     Vector.getOpcode() == AMDGPUISD::BUILD_VERTICAL_VECTOR)
//   //   return Op;

//   // Vector = vectorToVerticalVector(DAG, Vector);
//   // return DAG.getNode(ISD::EXTRACT_VECTOR_ELT, DL, Op.getValueType(),
//   //                    Vector, Index);
//   llvm::dbgs() << "LowerEXTRACT_VECTOR_ELT!!!";
//   return SDValue();
// }

// SDValue DRAITargetLowering::LowerConstantFP(SDValue Op,
//                                             SelectionDAG &DAG) const {
//   EVT VT = Op.getValueType();
//   ConstantFPSDNode *CFP = cast<ConstantFPSDNode>(Op);
//   const APFloat &FPVal = CFP->getValueAPF();
//   APInt INTVal = FPVal.bitcastToAPInt();

//   SDLoc DL(CFP);
//   switch (VT.getSimpleVT().SimpleTy) {
//   default:
//     llvm_unreachable("Unknown floating point type!");
//     break;
//   case MVT::f32:
//     return DAG.getNode(DRAIISD::ConstantFP, DL, MVT::f32, DAG.getConstant(INTVal, DL, MVT::i32));
//     // return DAG.getBitcast(MVT::f32, DAG.getConstant(INTVal, DL, MVT::i32));
//   }
// }

// SDValue DRAITargetLowering::LowerLOAD(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerLOAD\n";
//   Op.dump();

//   if (Op->getValueType(0) == MVT::i64) { // expand to i32 load x2
//     SDNode *N = Op.getNode();
//     SDLoc dl(N);

//     SDValue chain = N->getOperand(0);
//     SDValue addr = N->getOperand(1);
//     SDValue lower = DAG.getLoad(MVT::i32, dl, chain, addr, MachinePointerInfo());
//     SDValue offset = DAG.getConstant(4, dl, MVT::i64);
//     SDValue addr_up = DAG.getNode(ISD::ADD, dl, MVT::i64, addr, offset);
//     SDValue upper = DAG.getLoad(MVT::i32, dl, chain, addr_up, MachinePointerInfo());
//     SDValue result = DAG.getNode(ISD::BUILD_PAIR, dl, MVT::i64, lower, upper);
//     return result;
//   }

//   return SDValue();
// }

// SDValue DRAITargetLowering::LowerSTORE(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerSTORE\n";
//   Op.dump();
//   return Op;
// }

// SDValue DRAITargetLowering::LowerSIGN_EXTEND(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerSIGN_EXTEND!!!\n";

//   EVT type = Op->getValueType(0);
//   SDValue input = Op->getOperand(0);
//   if (type != Subtarget.Vec(MVT::i64) || input->getValueType(0) != Subtarget.Vec(MVT::i32)) {
//     return SDValue();
//   }

//   SDLoc dl(Op.getNode());
//   for (auto N : Op->uses()) {
//     if (N->getOpcode() == ISD::MGATHER) {
//       MaskedGatherSDNode *mga = cast<MaskedGatherSDNode>(N);
//       SmallVector<SDValue, 6> ops(N->op_begin(), N->op_end());
//       if (ops.size() < 5 || ops[4] != Op) {
//         llvm_unreachable("unexpected ops");
//       }
//       ops[4] = input; // direct use i32 index
//       SDValue new_op = DAG.getMaskedGather(
//           mga->getVTList(), mga->getMemoryVT(), dl, ops, mga->getMemOperand(),
//           mga->getIndexType(), mga->getExtensionType());
//       DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), new_op);
//     } else if (N->getOpcode() == ISD::MSCATTER) {
//       MaskedScatterSDNode *msc = cast<MaskedScatterSDNode>(N);
//       SmallVector<SDValue, 6> ops(N->op_begin(), N->op_end());
//       if (ops.size() < 5 || ops[4] != Op) {
//         llvm_unreachable("unexpected ops");
//       }
//       ops[4] = input; // direct use vi32 index
//       SDValue new_op =
//           DAG.getMaskedScatter(msc->getVTList(), msc->getMemoryVT(), dl, ops,
//                                msc->getMemOperand(), msc->getIndexType());
//       DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), new_op);
//     } else {
//       llvm_unreachable("unsupported consumer node");
//     }
//   }

//   DAG.RemoveDeadNode(Op.getNode());

//   return Op;
// }

// SDValue DRAITargetLowering::LowerMGATHER(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerMGATHER!!!\n";

//   SDNode *N = Op.getNode();
//   MaskedGatherSDNode *mga = cast<MaskedGatherSDNode>(N);
//   SmallVector<SDValue, 6> ops(N->op_begin(), N->op_end());
//   if (ops.size() < 5) {
//     llvm_unreachable("unexpected ops");
//   }

//   SDValue idx = ops[4];
//   if (idx->getValueType(0) != Subtarget.Vec(MVT::i64)) {
//     return SDValue();
//   }
//   if (idx->getOpcode() != ISD::SIGN_EXTEND) {
//     llvm_unreachable("unexpected index node");
//   }
//   SDValue idx32 = idx->getOperand(0);
//   if (idx32->getValueType(0) != Subtarget.Vec(MVT::i32)) {
//     llvm_unreachable("unexpected index type");
//   }

//   SDLoc dl(Op.getNode());
//   ops[4] = idx32;
//   SDValue new_op = DAG.getMaskedGather(
//       mga->getVTList(), mga->getMemoryVT(), dl, ops, mga->getMemOperand(),
//       mga->getIndexType(), mga->getExtensionType());

//   return new_op;
// }

// SDValue DRAITargetLowering::LowerMSCATTER(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerMSCATTER!!!\n";

//   SDNode *N = Op.getNode();
//   MaskedScatterSDNode *mga = cast<MaskedScatterSDNode>(N);
//   SmallVector<SDValue, 6> ops(N->op_begin(), N->op_end());
//   if (ops.size() < 5) {
//     llvm_unreachable("unexpected ops");
//   }

//   SDValue idx = ops[4];
//   if (idx->getValueType(0) != Subtarget.Vec(MVT::i64)) {
//     return SDValue();
//   }
//   if (idx->getOpcode() != ISD::SIGN_EXTEND) {
//     llvm_unreachable("unexpected index node");
//   }
//   SDValue idx32 = idx->getOperand(0);
//   if (idx32->getValueType(0) != Subtarget.Vec(MVT::i32)) {
//     llvm_unreachable("unexpected index type");
//   }

//   SDLoc dl(Op.getNode());
//   ops[4] = idx32;
//   SDValue new_op =
//       DAG.getMaskedScatter(mga->getVTList(), mga->getMemoryVT(), dl, ops,
//                            mga->getMemOperand(), mga->getIndexType());

//   return new_op;
// }

// SDValue DRAITargetLowering::LowerSHL(SDValue Op, SelectionDAG &DAG) const {
//   dbgs() << "hit: LowerSHL!!!\n";

//   EVT type = Op->getValueType(0);
//   if (type != MVT::i64) {
//     return SDValue();
//   }
//   SDValue input = Op->getOperand(0);
//   SDValue val = Op->getOperand(1);
//   if (val->getOpcode() != ISD::Constant) {
//     llvm_unreachable("unexpected shl value");
//   }

//   ConstantSDNode *c = cast<ConstantSDNode>(val.getNode());
//   int64_t cv = c->getConstantIntValue()->getSExtValue();
//   if (cv < 0) {
//     llvm_unreachable("unexpected shl value");
//   }

//   SDLoc dl(Op.getNode());
//   SDValue new_op = DAG.getNode(ISD::MUL, dl, type, input,
//                                DAG.getConstant((1LL << cv), dl, type));

//   return new_op;
// }

static SDValue LowerSIGN_EXTEND_INREG(SDValue Op, SelectionDAG &DAG) {
  dbgs() << "hit: LowerSIGN_EXTEND_INREG!!!\n";

  SDLoc dl(Op.getNode());
  EVT type = Op->getValueType(0);
  SDValue src = Op->getOperand(0);
  SDValue new_op = DAG.getNode(ISD::SIGN_EXTEND, dl, type, src);

  return new_op;
}

SDValue DRAITargetLowering::LowerOperation(SDValue Op,
                                           SelectionDAG &DAG) const {
  dbgs() << "LowerOperation!!! \n";
  Op.dump();

  switch (Op.getOpcode()) {
  default:
    break;
  // case ISD::ConstantFP:
  //   return LowerConstantFP(Op, DAG);
  // case ISD::LOAD:
  //   return LowerLOAD(Op, DAG);
  // case ISD::STORE:
  //   return LowerSTORE(Op, DAG);
  // case ISD::MGATHER:
  //   return LowerMGATHER(Op, DAG);
  // case ISD::MSCATTER:
  //   return LowerMSCATTER(Op, DAG);
  // case ISD::SHL:
  //   return LowerSHL(Op, DAG);
    /*
    replacing:
      i64 = sign_extend_inreg t20, ValueType:ch:i32
    with:
      i64 = sign_extend t20
    */
    case ISD::SIGN_EXTEND_INREG:
      return LowerSIGN_EXTEND_INREG(Op, DAG);
  }

  return SDValue();
}

void DRAITargetLowering::ReplaceNodeResults(SDNode *N,
                                            SmallVectorImpl<SDValue> &Results,
                                            SelectionDAG &DAG) const {
  dbgs() << "ReplaceNodeResults!!! \n";
  N->dump();

  /// This callback is invoked when a node result type is illegal for the
  /// target, and the operation was registered to use 'custom' lowering for that
  /// result type.  The target places new result values for the node in Results
  /// (their number and types must exactly match those of the original return
  /// values of the node), or leaves Results empty, which indicates that the
  /// node is not to be custom lowered after all.

  switch (N->getOpcode()) {
  default:
    break;
  // case ISD::SIGN_EXTEND:
  //   LowerSIGN_EXTEND(SDValue(N, 0), DAG);
  //   return;
  }
}

static SDValue performBRCONDCombine(SDNode *N,
                                    TargetLowering::DAGCombinerInfo &DCI,
                                    const DRAISubtarget &Subtarget) {
  SDLoc dl(N);
  SelectionDAG &DAG = DCI.DAG;

  SDValue cmp = N->getOperand(1);
  if (cmp.getOpcode() != ISD::SETCC) {
    return SDValue();
  }
  SDValue bcast = cmp->getOperand(0);
  SDValue cval = cmp->getOperand(1);
  SDValue cond = cmp->getOperand(2);
  if (bcast.getOpcode() != ISD::BITCAST || !isa<ConstantSDNode>(cval) ||
      !isa<CondCodeSDNode>(cond)) {
    return SDValue();
  }
  SDValue mask = bcast->getOperand(0);
  if (mask->getValueType(0) != Subtarget.Vec(MVT::i1)) {
    return SDValue();
  }

  int64_t val = cast<ConstantSDNode>(cval)->getSExtValue();
  ISD::CondCode ccode = cast<CondCodeSDNode>(cond)->get();
  unsigned jmp_code = 0;
  if (ccode == ISD::SETEQ && val == -1) {
    jmp_code = DRAIISD::JmpAll;
  } else if (ccode == ISD::SETNE && val == 0) {
    jmp_code = DRAIISD::JmpAny;
  } else if (ccode == ISD::SETEQ && val == 0) {
    jmp_code = DRAIISD::JmpNone;
  } else {
    return SDValue(); // failed
  }

  SDValue chain = N->getOperand(0);
  SDValue jmp =
      DAG.getNode(jmp_code, dl, MVT::i1, chain, mask, N->getOperand(2));
  DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), jmp);

  return SDValue(N, 0); // success
}

static SDValue performZERO_EXTENDCombine(SDNode *N,
                                         TargetLowering::DAGCombinerInfo &DCI,
                                         const DRAISubtarget &Subtarget) {
  SDLoc dl(N);
  SelectionDAG &DAG = DCI.DAG;

  EVT type = N->getValueType(0);
  if (type != MVT::i32 && type != MVT::i64 && type != Subtarget.Vec(MVT::i32)) {
    return SDValue();
  }

  SDValue mask = N->getOperand(0);
  if (mask->getValueType(0) != MVT::i1 && mask->getValueType(0) != Subtarget.Vec(MVT::i1)) {
    return SDValue();
  }

  MVT elem_type = (type == MVT::i64) ? MVT::i64 : MVT::i32;
  SDValue zero = DAG.getConstant(0, dl, elem_type);
  SDValue one = DAG.getConstant(1, dl, elem_type);
  if (type == Subtarget.Vec(MVT::i32)) {
    SmallVector<SDValue, 8> zops(Subtarget.GetDRAISimdWidth(), zero);
    zero = DAG.getBuildVector(Subtarget.Vec(MVT::i32), dl, zops);
    SmallVector<SDValue, 8> oops(Subtarget.GetDRAISimdWidth(), one);
    one = DAG.getBuildVector(Subtarget.Vec(MVT::i32), dl, oops);
  }

  SDValue sel = DAG.getSelect(dl, type, mask, one, zero);
  DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), sel);

  return SDValue(N, 0);
}

// static SDValue performConstantCombine(SDNode *N,
//                                       TargetLowering::DAGCombinerInfo &DCI) {
//   SDLoc dl(N);
//   SelectionDAG &DAG = DCI.DAG;

//   EVT type = N->getValueType(0);
//   if (type != MVT::i64) {
//     return SDValue();
//   }

//   // combine to constant32 + sext
//   ConstantSDNode *c = cast<ConstantSDNode>(N);
//   int64_t val = c->getSExtValue();
//   if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max()) {
//     llvm_unreachable("constant value exceeded int value range!");
//   }
//   SDValue result = DAG.getTargetConstant(val, dl, MVT::i64);
//   DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), result);

//   return SDValue(N, 0);
// }

// static SDValue performSHLCombine(SDNode *N,
//                                  TargetLowering::DAGCombinerInfo &DCI) {
//   dbgs() << "hit: performSHLCombine!!!\n";

//   EVT type = N->getValueType(0);
//   if (type != MVT::i64) {
//     return SDValue();
//   }
//   SDValue input = N->getOperand(0);
//   SDValue val = N->getOperand(1);
//   if (val->getOpcode() != ISD::Constant) {
//     llvm_unreachable("unexpected shl value");
//   }

//   ConstantSDNode *c = cast<ConstantSDNode>(val.getNode());
//   int64_t cv = c->getConstantIntValue()->getSExtValue();
//   if (cv < 0) {
//     llvm_unreachable("unexpected shl value");
//   }

//   SDLoc dl(N);
//   SelectionDAG &DAG = DCI.DAG;
//   SDValue new_op = DAG.getNode(ISD::MUL, dl, type, input,
//                                DAG.getConstant((1LL << cv), dl, type));

//   return new_op;
// }

/*
static SDValue performLoopCombine(SDNode *N,
                                  TargetLowering::DAGCombinerInfo &DCI,
                                  uint64_t intr_id) {
  SDLoc dl(N);
  SelectionDAG &DAG = DCI.DAG;

  if (intr_id == Intrinsic::drai_loop_begin) {
    // find lp_header & lp_end
    SDNode *condbr = nullptr;
    for (auto U : N->uses()) {
      if (U->getOpcode() == ISD::BRCOND) {
        condbr = U;
      }
    }
    if (!condbr) {
      llvm_unreachable("condbr not found!!");
    }
    SDNode *br = nullptr;
    for (auto U : condbr->uses()) {
      if (U->getOpcode() == ISD::BR) {
        br = U;
      }
    }
    if (!br) {
      llvm_unreachable("br not found!!");
    }
    SDValue cond = condbr->getOperand(1);
    SDValue lp_header, lp_end;
    if (cond.getNode() == N) {
      lp_header = condbr->getOperand(2);
      lp_end = br->getOperand(1);
    } else if (cond->getOpcode() == ISD::SETCC &&
               cond->getOperand(0).getNode() == N) {
      if (!isa<ConstantSDNode>(cond->getOperand(1)) ||
          cast<ConstantSDNode>(cond->getOperand(1))->getSExtValue() != -1 ||
          cast<CondCodeSDNode>(cond->getOperand(2))->get() != ISD::SETNE) {
        llvm_unreachable("unexpected cond!!");
      }
      lp_end = condbr->getOperand(2);
      lp_header = br->getOperand(1);
    } else {
      llvm_unreachable("unexpected cond!!");
    }

    // if (condbr->getOperand(0) != SDValue(N, 1)) { // chain
    //   llvm_unreachable("unexpected condbr's chain!!");
    // }

    // create loop & update br to lp_header
    SDValue lp_count = N->getOperand(2); // operand1 is TargetConstant
    SDValue loop =
        DAG.getNode(DRAIISD::Loop, dl, MVT::Other, condbr->getOperand(0), lp_count, lp_end);
    SDValue new_br = DAG.getNode(ISD::BR, dl, MVT::Other, loop, lp_header);

    // replace chain
    DAG.ReplaceAllUsesOfValueWith(SDValue(N, 1), N->getOperand(0));
    DAG.ReplaceAllUsesOfValueWith(SDValue(condbr, 0), loop);
    DAG.ReplaceAllUsesOfValueWith(SDValue(br, 0), new_br);
  } else if (intr_id == Intrinsic::drai_loop_dec) {
    SDNode *condbr = nullptr;
    for (auto U : N->uses()) {
      if (U->getOpcode() == ISD::BRCOND) {
        condbr = U;
      }
    }
    if (!condbr) {
      llvm_unreachable("condbr not found!!");
    }
    if (condbr->getOperand(1) != SDValue(N, 0)) {
      llvm_unreachable("unexpected cond!!");
    }

    // remove loop.dec & condbr
    DAG.ReplaceAllUsesOfValueWith(SDValue(N, 1), N->getOperand(0)); // SDValue(N, 1) is output chain
    DAG.ReplaceAllUsesOfValueWith(SDValue(condbr, 0), condbr->getOperand(0));
    // automatic remove unsed nodes
  } else {
    llvm_unreachable("unexpect!!");
  }

  return SDValue(N, 0);
}
*/

static SDValue performGlobalAddressCombine(SDNode *N,
                                           TargetLowering::DAGCombinerInfo &DCI,
                                           const DRAISubtarget &Subtarget) {
  dbgs() << "hit: performGlobalAddressCombine!!!\n";
  SDLoc dl(N);
  SelectionDAG &DAG = DCI.DAG;
  auto *GN = cast<GlobalAddressSDNode>(N);

  unsigned AS = GN->getAddressSpace();
  if (AS == (unsigned)DRAIAddressSpace::ADDRSPACE_SHARED) {
    // shared memory base address alway is 0, but an imm offset maybe included
    return DAG.getConstant(GN->getOffset(), dl, MVT::i64);
  }

  return SDValue();
}

SDValue DRAITargetLowering::PerformDAGCombine(SDNode *N,
                                              DAGCombinerInfo &DCI) const {
  unsigned Opc = N->getOpcode();

  if (DCI.isBeforeLegalizeOps()) {
    LLVM_DEBUG(dbgs() << "isBeforeLegalizeOps\n");

    switch (Opc) {
    default:
      break;
    case ISD::GlobalAddress:
      return performGlobalAddressCombine(N, DCI, Subtarget);
    }

    return SDValue();
  }

  LLVM_DEBUG(dbgs() << "In PerformDAGCombine\n");

  switch (Opc) {
  default:
    break;
  case ISD::BRCOND:
    return performBRCONDCombine(N, DCI, Subtarget);
  case ISD::ZERO_EXTEND: // todo use cast to replace this
    return performZERO_EXTENDCombine(N, DCI, Subtarget);
  // case ISD::Constant:
  //   return performConstantCombine(N, DCI);
  // case ISD::SHL:
  //   return performSHLCombine(N, DCI);
  case ISD::INTRINSIC_W_CHAIN: {
    uint64_t intr_id = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
    switch (intr_id) {
    default:
      break;
    // case Intrinsic::drai_loop_begin:
    // case Intrinsic::drai_loop_dec:
    //   return performLoopCombine(N, DCI, intr_id);
    };
  } break;
  }

  return SDValue();
}

//===----------------------------------------------------------------------===//
// Calling conventions
//===----------------------------------------------------------------------===//

#include "DRAIGenCallingConv.inc"

SDValue DRAITargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_DRAI);

  int arg_offset = 0;
  for (unsigned I = 0, E = ArgLocs.size(); I != E; ++I) {
    SDValue ArgValue;
    CCValAssign &VA = ArgLocs[I];
    EVT LocVT = VA.getLocVT();
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      const TargetRegisterClass *RC;
      switch (LocVT.getSimpleVT().SimpleTy) {
      default:
        // Integers smaller than i64 should be promoted to i32.
        llvm_unreachable("Unexpected argument type");
      case MVT::i32:
        RC = &DRAI::SRegRegClass;
        break;
      }

      Register VReg = MRI.createVirtualRegister(RC);
      MRI.addLiveIn(VA.getLocReg(), VReg);
      ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);

      // If this is an 8/16-bit value, it is really passed promoted to 32
      // bits. Insert an assert[sz]ext to capture this, then truncate to the
      // right size.
      if (VA.getLocInfo() == CCValAssign::SExt)
        ArgValue = DAG.getNode(ISD::AssertSext, DL, LocVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));
      else if (VA.getLocInfo() == CCValAssign::ZExt)
        ArgValue = DAG.getNode(ISD::AssertZext, DL, LocVT, ArgValue,
                               DAG.getValueType(VA.getValVT()));

      if (VA.getLocInfo() != CCValAssign::Full)
        ArgValue = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), ArgValue);

      InVals.push_back(ArgValue);
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");

      // all register are loaded from the sram, %arg be argument base address
      Register Reg = MF.addLiveIn(DRAI::ArgBase, &DRAI::ERegRegClass);
      SDValue arg_base = DAG.getCopyFromReg(Chain, DL, Reg, MVT::i64);
      SDValue ptr = DAG.getNode(ISD::ADD, DL, MVT::i64, arg_base,
                                DAG.getConstant(arg_offset, DL, MVT::i64));
      SDValue value =
          DAG.getLoad(VA.getValVT(), DL, Chain, ptr, MachinePointerInfo());
      InVals.push_back(value);

      // arg_offset += VA.getValVT().getSizeInBits() / 8;
      arg_offset += 8; // always 8 bytes align
    }
  }

  if (IsVarArg) {
    llvm_unreachable("DRAI - LowerFormalArguments - VarArgs not Implemented");
  }

  return Chain;
}

SDValue
DRAITargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SDLoc &DL, SelectionDAG &DAG) const {

  SmallVector<CCValAssign, 16> RVLocs;
  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analyze return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_DRAI);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // if (MF.getFunction().hasStructRetAttr()) {
  //   DRAIMachineFunctionInfo *VFI = MF.getInfo<DRAIMachineFunctionInfo>();
  //   unsigned Reg = VFI->getSRetReturnReg();
  //   if (!Reg)
  //     llvm_unreachable("sret virtual register not created in the entry block");
  //   SDValue Val =
  //     DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
  //   Chain = DAG.getCopyToReg(Chain, DL, Nyuzi::S0, Val, Flag);
  //   Flag = Chain.getValue(1);
  //   RetOps.push_back(
  //       DAG.getRegister(Nyuzi::SR60, getPointerTy(DAG.getDataLayout())));
  // }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(DRAIISD::RET_FLAG, DL, MVT::Other, RetOps);

}

SDValue DRAITargetLowering::LowerCall(CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const {
  llvm_unreachable("DRAI - LowerCall - Not Implemented");
}

const char *DRAITargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
#define OPCODE(Opc)                                                            \
  case Opc:                                                                    \
    return #Opc
    OPCODE(DRAIISD::RET_FLAG);
    // OPCODE(DRAIISD::CALL);
    // OPCODE(DRAIISD::CLR);
    // OPCODE(DRAIISD::SET);
    // OPCODE(DRAIISD::EXT);
    // OPCODE(DRAIISD::EXTU);
    // OPCODE(DRAIISD::MAK);
    // OPCODE(DRAIISD::ROT);
    // OPCODE(DRAIISD::FF1);
    // OPCODE(DRAIISD::FF0);
    // OPCODE(DRAIISD::ConstantFP);
    OPCODE(DRAIISD::JmpAll);
    OPCODE(DRAIISD::JmpAny);
    OPCODE(DRAIISD::JmpNone);
    OPCODE(DRAIISD::Loop);
#undef OPCODE
  default:
    return nullptr;
  }
}

std::pair<unsigned, const TargetRegisterClass *>
DRAITargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint,
                                                 MVT VT) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:
      llvm_unreachable("unsupported inline asm constraint!!\n");
    case 'r':
      if (VT == MVT::i32 || VT == MVT::f16 || VT == MVT::f32) {
        return std::make_pair(0U, &DRAI::SRegRegClass);
      } else if (VT == Subtarget.Vec(MVT::i32) ||
                 VT == Subtarget.Vec(MVT::f16) ||
                 VT == Subtarget.Vec(MVT::f32)) {
        return std::make_pair(0U, &DRAI::VRegRegClass);
      } else if (VT == MVT::i64) {
        return std::make_pair(0U, &DRAI::ERegRegClass);
      } else if (VT == Subtarget.Vec(MVT::i1)) {
        return std::make_pair(0U, &DRAI::PRegRegClass);
      } else {
        llvm_unreachable("unexpect VT!!\n");
      }
      break;
    }
  }

  // todo support specified physical register

  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

EVT DRAITargetLowering::getSetCCResultType(const DataLayout &,
                                           LLVMContext &C, EVT VT) const {
  if (!VT.isVector())
    return MVT::i1;
  return EVT::getVectorVT(C, MVT::i1, VT.getVectorElementCount());
}

MVT DRAITargetLowering::getScalarShiftAmountTy(const DataLayout &DL,
                                               EVT LHSTy) const {
  // fix: Creating new node: t23: i32 = srl t20, Constant:i64<5>
  // to:  Creating new node: t23: i32 = srl t20, Constant:i32<5>
  return LHSTy.getSimpleVT();
}

TargetLoweringBase::LegalizeTypeAction
DRAITargetLowering::getPreferredVectorAction(MVT VT) const {
  return TargetLoweringBase::getPreferredVectorAction(VT);
}

bool DRAITargetLowering::getTgtMemIntrinsic(IntrinsicInfo &Info,
                                            const CallInst &I,
                                            MachineFunction &MF,
                                            unsigned Intrinsic) const {
  auto &DL = I.getModule()->getDataLayout();

  switch (Intrinsic) {
  case Intrinsic::drai_scatter_store_i:
  case Intrinsic::drai_scatter_store_f:
  case Intrinsic::drai_scatter_store_h: {
    Type *ValTy =
        cast<VectorType>(I.getArgOperand(2)->getType())->getElementType();

    Info.opc = ISD::INTRINSIC_VOID;
    Info.memVT = MVT::getVT(ValTy);
    Info.ptrVal = I.getArgOperand(0);
    Info.offset = 0;
    Info.align = DL.getABITypeAlign(ValTy);
    Info.flags = MachineMemOperand::MOLoad | MachineMemOperand::MOStore;
    return true;
  }
  default:
    return false;
  }
}