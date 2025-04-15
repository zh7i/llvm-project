#include "DRAIPasses.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
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
#include <atomic>
#include <unordered_map>
using namespace llvm;

#define DEBUG_TYPE "drai-cvt-hardware-loops"

static IntrinsicInst *getDRAISetIter(Loop *L) {
  BasicBlock *preheader = L->getLoopPreheader();
  if (!preheader) {
    llvm::dbgs() << "loop preheader not found!!";
    return nullptr;
  }
  IntrinsicInst *set_iter = nullptr;
  for (Instruction &I : llvm::make_early_inc_range(*preheader)) {
    if (auto *II = dyn_cast<IntrinsicInst>(&I)) {
      if (II->getIntrinsicID() == Intrinsic::set_loop_iterations) {
        if (set_iter) {
          llvm_unreachable(
              "has mutiple llvm.set.loop.iterations in preheader!!");
        }
        set_iter = II;
      }
    }
  }
  return set_iter;
}

static IntrinsicInst *getDRAIIterDec(Loop *L) {
  BasicBlock *lp_latch = L->getLoopLatch();
  if (!lp_latch) {
    llvm_unreachable("loop latch not found!!");
  }
  BranchInst *lp_condbr = dyn_cast<BranchInst>(lp_latch->getTerminator());
  IntrinsicInst *iter_dec = dyn_cast<IntrinsicInst>(lp_condbr->getCondition());
  return iter_dec;
}

static bool convertIntrinsic(Loop *L, uint32_t depth) {
  IntrinsicInst *set_iter = getDRAISetIter(L);
  if (!set_iter) {
    return false;
  }

  IntrinsicInst *iter_dec = getDRAIIterDec(L);
  if (!iter_dec) {
    llvm_unreachable("llvm.loop.dec not found!!");
  }

  dbgs() << "processLoop: " << L->getHeader()->getName() << "\n";

  // create drai loop intrinsic
  static std::atomic<uint32_t> hw_loop_id;
  uint32_t id = hw_loop_id.fetch_add(1);

  IRBuilder<> B(set_iter);
  ConstantInt *id_c = ConstantInt::get(B.getInt32Ty(), id);
  Value *lp_count = set_iter->getOperand(0);
  ConstantInt *depth_c = ConstantInt::get(B.getInt32Ty(), depth);
  CallInst *new_set_iter = B.CreateIntrinsic(
      Intrinsic::drai_loop_set_iter, {}, // not overloaded
      {id_c, depth_c, lp_count}, nullptr, set_iter->getName());
  set_iter->replaceAllUsesWith(new_set_iter);
  set_iter->eraseFromParent();

  B.SetInsertPoint(iter_dec);
  CallInst *new_iter_dec =
      B.CreateIntrinsic(Intrinsic::drai_loop_iter_dec, {}, // not overloaded
                        id_c, nullptr, set_iter->getName());
  iter_dec->replaceAllUsesWith(new_iter_dec);
  iter_dec->eraseFromParent();

  return true;
}

static bool traverseLoop(Loop *L, LoopInfo *LI, DominatorTree *DT,
                         uint32_t depth, std::vector<Loop *> &cvted_loops) {

  bool curr = convertIntrinsic(L, depth);
  bool changed = curr;

  for (Loop *SL : *L) {
    changed |= traverseLoop(SL, LI, DT, depth + curr, cvted_loops);
  }

  if (curr) {
    cvted_loops.push_back(L);
  }

  return changed;
}

static bool runImplWithLoopInfo(Function &F, LoopInfo *LI, DominatorTree *DT) {
  dbgs() << "CvtHardwareLoopsPass::runImplWithLoopInfo!!!!\n";
  bool changed = false;

  std::vector<Loop *> cvted_loops;

  for (Loop *L : *LI) {
    if (L->isOutermost()) {
      changed |= traverseLoop(L, LI, DT, 0, cvted_loops);
    }
  }

  return changed;
}

namespace {

class DRAICvtHardwareLoops : public FunctionPass {
public:
  static char ID;

  DRAICvtHardwareLoops() : FunctionPass(ID) {
    initializeDRAICvtHardwareLoopsPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addPreserved<DominatorTreeWrapperPass>();
  }

  StringRef getPassName() const override {
    return "DRAI Cvt Hardware Loops Function Pass";
  }
};

bool DRAICvtHardwareLoops::runOnFunction(Function &F) {
  llvm::dbgs() << "DRAICvtHardwareLoops::runOnFunction hit!!!\n";

  auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();

  bool changed = runImplWithLoopInfo(F, &LI, &DT);
  return changed;
}

} // namespace

INITIALIZE_PASS(DRAICvtHardwareLoops, DEBUG_TYPE,
                "DRAI Cvt Hardware Loops Function Pass", false, false)

char DRAICvtHardwareLoops::ID = 0;
FunctionPass *llvm::createDRAICvtHardwareLoopsPass() {
  return new DRAICvtHardwareLoops();
}
