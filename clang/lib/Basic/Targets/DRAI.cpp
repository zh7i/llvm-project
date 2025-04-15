//===--- DRAI.cpp - Implement DRAI target feature support ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements DRAI TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "DRAI.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"

namespace clang {
namespace targets {

DRAITargetInfo::DRAITargetInfo(const llvm::Triple &Triple,
                               const TargetOptions &)
    : TargetInfo(Triple) {
  BigEndian = false;
  TLSSupported = false;
  IntWidth = IntAlign = 32;
  PointerWidth = PointerAlign = 64;
  SizeType = UnsignedInt;
  PtrDiffType = SignedInt;
  MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 32;
  // Pointer's size, abi align, preferred align are 64 bit, Indexs are 32bit.
  // parse in DataLayout::parseSpecifier
  resetDataLayout("e-m:e-p:64:64:64:32-n32");
  HasFloat16 = true;
  LongDoubleWidth = LongDoubleAlign = 32;
  DoubleWidth = DoubleAlign = 32;
  DoubleFormat = &llvm::APFloat::IEEEsingle();
  LongDoubleFormat = &llvm::APFloat::IEEEsingle();
}

void DRAITargetInfo::getTargetDefines(const LangOptions &Opts,
                                      MacroBuilder &Builder) const {
  Builder.defineMacro("DRAI_COMPILER");
  Builder.defineMacro("DRAI_SIMD_WIDTH", std::to_string(GetDRAISimdWidth()));
}

bool DRAITargetInfo::setCPU(const std::string &Name) {
  return Name == "drai001";
}

ArrayRef<TargetInfo::GCCRegAlias> DRAITargetInfo::getGCCRegAliases() const {
  return std::nullopt;
}

std::string_view DRAITargetInfo::getClobbers() const { return ""; }

TargetInfo::BuiltinVaListKind DRAITargetInfo::getBuiltinVaListKind() const {
  return TargetInfo::VoidPtrBuiltinVaList;
}

const char *const DRAITargetInfo::GCCRegNames[] = {
    "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",  "s8",  "s9",  "s10",
    "s11", "s12", "s13", "s14", "s15", "s16", "s17", "s18", "s19", "s20", "s21",
    "s22", "s23", "s24", "s25", "s26", "s27", "s28", "fp",  "sp",  "ra",  "v0",
    "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",  "v8",  "v9",  "v10", "v11",
    "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22",
    "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",
};

const Builtin::Info DRAITargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, HeaderDesc::NO_HEADER, ALL_LANGUAGES},
#define TARGET_BUILTIN(ID, TYPE, ATTRS, FEATURE)                               \
  {#ID, TYPE, ATTRS, FEATURE, HeaderDesc::NO_HEADER, ALL_LANGUAGES},
#include "clang/Basic/BuiltinsDRAI.def"
};

ArrayRef<const char *> DRAITargetInfo::getGCCRegNames() const {
  return llvm::ArrayRef(GCCRegNames);
}

bool DRAITargetInfo::validateAsmConstraint(
    const char *&Name, TargetInfo::ConstraintInfo &Info) const {
  switch (*Name) {
  default:
    return false;

  case 's':
  case 'v':
    Info.setAllowsRegister();
    return true;

  case 'I': // Unsigned 8-bit constant
  case 'J': // Unsigned 12-bit constant
  case 'K': // Signed 16-bit constant
  case 'L': // Signed 20-bit displacement (on all targets we support)
  case 'M': // 0x7fffffff
    return true;

  case 'Q': // Memory with base and unsigned 12-bit displacement
  case 'R': // Likewise, plus an index
  case 'S': // Memory with base and signed 20-bit displacement
  case 'T': // Likewise, plus an index
    Info.setAllowsMemory();
    return true;
  }
}

ArrayRef<Builtin::Info> DRAITargetInfo::getTargetBuiltins() const {
  return llvm::ArrayRef(BuiltinInfo,
                        clang::DRAI::LastTSBuiltin - Builtin::FirstTSBuiltin);
}

uint32_t DRAITargetInfo::GetDRAISimdWidth() const { return 64; }

} // namespace targets
} // namespace clang