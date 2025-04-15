#include "DRAIPasses.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicsDRAI.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/ADCE.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
using namespace llvm;

#define DEBUG_TYPE "drai-block-reorder"
#define DEBUG_TYPE_MACHINE "drai-machine-block-reorder"

static bool reorderFunctionBlocks(Function &F) {
  bool changed = false;

  ReversePostOrderTraversal<Function *> RPOT(&F);
  auto curr = F.begin();
  for (BasicBlock *B : RPOT) {
    auto ordered = B->getIterator();
    if (curr != ordered) {
      F.splice(curr, &F, ordered);
      curr = ordered; // invasive list node
      assert(curr == B->getIterator());
      changed = true;
    }
    ++curr;
  }

  return changed;
}

static bool reorderMachineFunctionBlocks(MachineFunction &F) {
  bool changed = false;

  ReversePostOrderTraversal<MachineFunction *> RPOT(&F);
  auto curr = F.begin();
  for (MachineBasicBlock *B : RPOT) {
    auto ordered = B->getIterator();
    if (curr != ordered) {
      F.splice(curr, ordered);
      curr = ordered; // invasive list node
      assert(curr == B->getIterator());
      changed = true;
    }
    ++curr;
  }

  return changed;
}

namespace {
class DRAIBlockReorder : public FunctionPass {
public:
  static char ID;

  DRAIBlockReorder() : FunctionPass(ID) {
    initializeDRAIBlockReorderPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override {
    llvm::dbgs() << "DRAIBlockReorder::runOnFunction hit!!!\n";

    return reorderFunctionBlocks(F);
  }

  StringRef getPassName() const override {
    return "DRAI Function Block Reorder Pass";
  }
};

class DRAIMachineBlockReorder : public MachineFunctionPass {
public:
  static char ID;

  DRAIMachineBlockReorder() : MachineFunctionPass(ID) {
    initializeDRAIMachineBlockReorderPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    llvm::dbgs() << "DRAIMachineBlockReorder::runOnFunction hit!!!\n";

    return reorderMachineFunctionBlocks(MF);
  }

  StringRef getPassName() const override {
    return "DRAI Machine Function Block Reorder Pass";
  }
};
} // namespace

INITIALIZE_PASS(DRAIBlockReorder, DEBUG_TYPE,
                "DRAI Function Block Reorder Pass", false, false)

char DRAIBlockReorder::ID = 0;
FunctionPass *llvm::createDRAIBlockReorderPass() {
  return new DRAIBlockReorder();
}

INITIALIZE_PASS(DRAIMachineBlockReorder, DEBUG_TYPE_MACHINE,
                "DRAI Machine Function Block Reorder Pass", false, false)

char DRAIMachineBlockReorder::ID = 0;
FunctionPass *llvm::createDRAIMachineBlockReorderPass() {
  return new DRAIMachineBlockReorder();
}
