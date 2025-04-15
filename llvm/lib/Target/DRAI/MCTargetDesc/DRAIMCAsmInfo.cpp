//===-- DRAIMCAsmInfo.cpp - DRAI asm properties ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DRAIMCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"

using namespace llvm;

DRAIMCAsmInfo::DRAIMCAsmInfo(const Triple &TT) {
  // TODO: Check!
  // CodePointerSize = 4;
  // CalleeSaveStackSlotSize = 4;
  IsLittleEndian = true;
  SupportsDebugInformation = true;
  SupportsDebugInformation = true;

  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
  // UseDotAlignForAlignment = true;
  // MinInstAlignment = 4;

  // CommentString = "#";
  // ZeroDirective = "\t.space\t";
  // Data64bitsDirective = "\t.quad\t";
  // UsesELFSectionDirectiveForBSS = true;
  // SupportsDebugInformation = true;
  // ExceptionsType = ExceptionHandling::DwarfCFI;
}
