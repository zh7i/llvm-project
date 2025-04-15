#ifndef LLVM_LIB_TARGET_DRAI_DRAI_H
#define LLVM_LIB_TARGET_DRAI_DRAI_H

#include "DRAIBaseInfo.h"
#include "MCTargetDesc/DRAIMCTargetDesc.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class DRAITargetMachine;
class FunctionPass;

FunctionPass *createDRAIISelDag(DRAITargetMachine &TM,
                                CodeGenOpt::Level OptLevel);
} // end namespace llvm
#endif
