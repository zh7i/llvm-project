//===-- DRAITargetInfo.cpp - DRAI target implementation -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/DRAITargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheDRAITarget() {
  static Target TheDRAITarget;
  return TheDRAITarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDRAITargetInfo() {
  RegisterTarget<Triple::drai, /*HasJIT=*/false> X(getTheDRAITarget(), "drai",
                                                   "DRAI", "DRAI");
}
