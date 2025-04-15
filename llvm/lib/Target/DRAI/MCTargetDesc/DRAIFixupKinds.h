//===-- DRAIFixupKinds.h - DRAI Specific Fixup Entries ----------*- C++ -*-===//
//
//                    The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DRAI_MCTARGETDESC_DRAIFIXUPKINDS_H
#define LLVM_LIB_TARGET_DRAI_MCTARGETDESC_DRAIFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace DRAI {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.

  // This table *must* be in the save order of
  // MCFixupKindInfo Infos[DRAI::NumTargetFixupKinds]
  // in DRAIAsmBackend.cpp.
  enum Fixups {
    // PC relative branch fixup resulting
    FixupRelPC32 = FirstTargetFixupKind,

    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
} // namespace DRAI
} // namespace llvm

#endif
