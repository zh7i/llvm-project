//===-- DRAITargetTransformInfo.cpp - DRAI specific TTI pass ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements a TargetTransformInfo analysis pass specific to the
/// DRAI target machine. It uses the target's detailed information to provide
/// more precise answers to certain TTI queries, while letting the target
/// independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//
/// About Cost Model numbers used below it's necessary to say the following:
/// the numbers correspond to some "generic" DRAI CPU instead of usage of
/// concrete CPU model. Usually the numbers correspond to CPU where the feature
/// apeared at the first time. For example, if we do Subtarget.hasSSE42() in
/// the lookups below the cost is based on Nehalem as that was the first CPU
/// to support that feature level and thus has most likely the worst case cost.
/// Some examples of other technologies/CPUs:
///   SSE 3   - Pentium4 / Athlon64
///   SSE 4.1 - Penryn
///   SSE 4.2 - Nehalem
///   AVX     - Sandy Bridge
///   AVX2    - Haswell
///   AVX-512 - Xeon Phi / Skylake
/// And some examples of instruction target dependent costs (latency)
///                   divss     sqrtss          rsqrtss
///   AMD K7            11-16     19              3
///   Piledriver        9-24      13-15           5
///   Jaguar            14        16              2
///   Pentium II,III    18        30              2
///   Nehalem           7-14      7-18            3
///   Haswell           10-13     11              5
/// TODO: Develop and implement  the target dependent cost model and
/// specialize cost numbers for different Cost Model Targets such as throughput,
/// code size, latency and uop count.
//===----------------------------------------------------------------------===//

#include "DRAITargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/CodeGen/CostTable.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IntrinsicsDRAI.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "DRAItti"

bool DRAITTIImpl::isLegalMaskedLoad(Type *DataType, Align Alignment) {
  return true;
}

bool DRAITTIImpl::isLegalMaskedStore(Type *DataType, Align Alignment) {
  return true;
}

bool DRAITTIImpl::isLegalMaskedGather(Type *DataType, Align Alignment) {
  return true;
}

bool DRAITTIImpl::isLegalMaskedScatter(Type *DataType, Align Alignment) {
  return true;
}

bool DRAITTIImpl::shouldExpandReduction(const IntrinsicInst *II) const {
  return false;
}

TargetTransformInfo::VPLegalization
DRAITTIImpl::getVPLegalizationStrategy(const VPIntrinsic &PI) const {
  return TargetTransformInfo::VPLegalization(
      TargetTransformInfo::VPLegalization::Legal,
      TargetTransformInfo::VPLegalization::Legal);
}

// bool DRAITTIImpl::isLegalMaskedGather(Type *DataTy, Align Alignment) {
//   // Some CPUs have better gather performance than others.
//   // TODO: Remove the explicit ST->hasAVX512()?, That would mean we would only
//   // enable gather with a -march.
//   if (!(ST->hasAVX512() || (ST->hasFastGather() && ST->hasAVX2())))
//     return false;

//   // This function is called now in two cases: from the Loop Vectorizer
//   // and from the Scalarizer.
//   // When the Loop Vectorizer asks about legality of the feature,
//   // the vectorization factor is not calculated yet. The Loop Vectorizer
//   // sends a scalar type and the decision is based on the width of the
//   // scalar element.
//   // Later on, the cost model will estimate usage this intrinsic based on
//   // the vector type.
//   // The Scalarizer asks again about legality. It sends a vector type.
//   // In this case we can reject non-power-of-2 vectors.
//   // We also reject single element vectors as the type legalizer can't
//   // scalarize it.
//   if (auto *DataVTy = dyn_cast<FixedVectorType>(DataTy)) {
//     unsigned NumElts = DataVTy->getNumElements();
//     if (NumElts == 1)
//       return false;
//   }
//   Type *ScalarTy = DataTy->getScalarType();
//   if (ScalarTy->isPointerTy())
//     return true;

//   if (ScalarTy->isFloatTy() || ScalarTy->isDoubleTy())
//     return true;

//   if (!ScalarTy->isIntegerTy())
//     return false;

//   unsigned IntWidth = ScalarTy->getIntegerBitWidth();
//   return IntWidth == 32 || IntWidth == 64;
// }

// bool DRAITTIImpl::isLegalMaskedScatter(Type *DataType, Align Alignment) {
//   // AVX2 doesn't support scatter
//   if (!ST->hasAVX512())
//     return false;
//   return isLegalMaskedGather(DataType, Alignment);
// }

bool DRAITTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                           AssumptionCache &AC,
                                           TargetLibraryInfo *LibInfo,
                                           HardwareLoopInfo &HWLoopInfo) {
  bool Enable = drai_hardware_loops;
  MDNode *LoopID = L->getLoopID();
  if (LoopID) {
    for (const MDOperand &MDO : LoopID->operands()) {
      if (auto MDN = dyn_cast<MDNode>(MDO)) {
        if (!MDN->getNumOperands()) {
          continue;
        }
        if (auto MD = dyn_cast<MDString>(MDN->getOperand(0))) {
          StringRef MDS = MD->getString();
          if (MDS == "llvm.drai.hwloop") {
            Enable = true;
          } else if (MDS == "llvm.drai.hwloop.disable") {
            Enable = false;
          }
        }
      }
    }
  }

  LLVMContext &C = L->getHeader()->getContext();
  HWLoopInfo.CounterInReg = false;
  HWLoopInfo.IsNestingLegal = true;
  HWLoopInfo.PerformEntryTest = false;
  HWLoopInfo.CountType = Type::getInt32Ty(C);
  HWLoopInfo.LoopDecrement = ConstantInt::get(HWLoopInfo.CountType, 1);
  return Enable;
}

bool DRAITTIImpl::collectFlatAddressOperands(SmallVectorImpl<int> &OpIndexes,
                                             Intrinsic::ID IID) const {
  switch (IID) {
  case Intrinsic::drai_scatter_store_i:
  case Intrinsic::drai_scatter_store_f:
  case Intrinsic::drai_scatter_store_h:
    OpIndexes.push_back(0); // pointer input index
    return true;
  default:
    return false;
  }
}

Value *DRAITTIImpl::rewriteIntrinsicWithAddressSpace(IntrinsicInst *II,
                                                     Value *OldV,
                                                     Value *NewV) const {
  Intrinsic::ID IID = II->getIntrinsicID();
  Module *M = II->getParent()->getParent()->getParent();

  switch (IID) {
  case Intrinsic::drai_scatter_store_i:
  case Intrinsic::drai_scatter_store_f:
  case Intrinsic::drai_scatter_store_h: {
    Type *NewPtrTy = NewV->getType();
    Function *NewDecl = Intrinsic::getDeclaration(M, IID, {NewPtrTy});
    II->setArgOperand(0, NewV);
    II->setCalledFunction(NewDecl);
    return II;
  }
  default:
    return nullptr;
  };
}

// bool DRAITTIImpl::getTgtMemIntrinsic(IntrinsicInst *Inst,
//                                      MemIntrinsicInfo &Info) const {
//   Intrinsic::ID IID = Inst->getIntrinsicID();

//   switch (IID) {
//   case Intrinsic::drai_scatter_store_i:
//   case Intrinsic::drai_scatter_store_f:
//   case Intrinsic::drai_scatter_store_h: {
//     Info.PtrVal = Inst->getArgOperand(0);
//     Info.ReadMem = true;
//     Info.WriteMem = true;
//     return true;
//   }
//   default:
//     return false;
//   }
// }