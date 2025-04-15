#ifndef LLVM_LIB_TARGET_DRAI_DRAI_PASSES_H
#define LLVM_LIB_TARGET_DRAI_DRAI_PASSES_H

#include "llvm/PassRegistry.h"

namespace llvm {
class FunctionPass;

// FunctionPass *createDRAIRemoveReduentLoadPass();

FunctionPass *createDRAILoopPseudoInstPass();

FunctionPass *createDRAICodeGenPreparePass();

FunctionPass *createDRAICvtHardwareLoopsPass();

FunctionPass *createDRAIBlockReorderPass();

FunctionPass *createDRAIMachineBlockReorderPass();

FunctionPass *createDRAIRemoveDupSelectPass();

void initializeDRAILoopPseudoInstPass(PassRegistry &);

void initializeDRAICodeGenPreparePass(PassRegistry &);

void initializeDRAICvtHardwareLoopsPass(PassRegistry &);

void initializeDRAIBlockReorderPass(PassRegistry &);

void initializeDRAIMachineBlockReorderPass(PassRegistry &);

void initializeDRAIRemoveDupSelectPass(PassRegistry &);

} // end namespace llvm

#endif
