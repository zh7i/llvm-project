//===-- DRAITargetTransformInfo.h - DRAI specific TTI -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file a TargetTransformInfo::Concept conforming object specific to the
/// DRAI target machine. It uses the target's detailed information to
/// provide more precise answers to certain TTI queries, while letting the
/// target independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DRAI_DRAITARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_DRAI_DRAITARGETTRANSFORMINFO_H

#include "DRAITargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class DRAITTIImpl : public BasicTTIImplBase<DRAITTIImpl> {
  typedef BasicTTIImplBase<DRAITTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const DRAISubtarget *ST;
  const DRAITargetLowering *TLI;

  const DRAISubtarget *getST() const { return ST; }
  const DRAITargetLowering *getTLI() const { return TLI; }

public:
  explicit DRAITTIImpl(const DRAITargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  // for (scalarize-masked-mem-intrin) pass
  bool isLegalMaskedLoad(Type *DataType, Align Alignment);
  bool isLegalMaskedStore(Type *DataType, Align Alignment);
  bool isLegalMaskedGather(Type *DataType, Align Alignment);
  bool isLegalMaskedScatter(Type *DataType, Align Alignment);

  // for (expand-reductions) pass
  bool shouldExpandReduction(const IntrinsicInst *II) const;

  // for (expandvp) pass
  TargetTransformInfo::VPLegalization
  getVPLegalizationStrategy(const VPIntrinsic &PI) const;

  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC,
                                TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);

  unsigned getFlatAddressSpace() const {
    return (unsigned)DRAIAddressSpace::ADDRSPACE_GLOBAL;
  }

  bool collectFlatAddressOperands(SmallVectorImpl<int> &OpIndexes,
                                  Intrinsic::ID IID) const;

  Value *rewriteIntrinsicWithAddressSpace(IntrinsicInst *II, Value *OldV,
                                          Value *NewV) const;

  // bool getTgtMemIntrinsic(IntrinsicInst *Inst, MemIntrinsicInfo &Info) const;
};

} // end namespace llvm

#endif
