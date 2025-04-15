#include "DRAI.h"
#include "DRAIInstrInfo.h"
#include "DRAIPasses.h"
#include "DRAITargetMachine.h"

#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/PassRegistry.h"

using namespace llvm;
using namespace PatternMatch;

#define DEBUG_TYPE "drai-remove-dup-select"

namespace {

class DRAIRemoveDupSelect : public FunctionPass {
public:
  static char ID;

  DRAIRemoveDupSelect() : FunctionPass(ID) {
    initializeDRAIRemoveDupSelectPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override;

  StringRef getPassName() const override {
    return "DRAI Remove Duplicate Select Function Pass";
  }

private:
  bool RemoveSimdBranchSelect(Instruction &I);
  bool RemoveSimdBranchSelect2(Instruction &I);
};

bool DRAIRemoveDupSelect::runOnFunction(Function &F) {
  llvm::dbgs() << "DRAIRemoveDupSelect::runOnFunction hit!!!\n";

  bool changed = false;

  for (BasicBlock &BB : llvm::make_early_inc_range(F)) {
    for (Instruction &I : llvm::make_early_inc_range(BB)) {
      changed |= RemoveSimdBranchSelect(I);
      changed |= RemoveSimdBranchSelect2(I);
    }
  }

  return changed;
}

bool DRAIRemoveDupSelect::RemoveSimdBranchSelect(Instruction &I) {
  /*
replacing:
  mask ---> bitcast --> ne 0 -
         \                    \
  new  ---> select1 -----------> select
         /                    /
  old  -----------------------

with:
  mask --
         \
  new  ---> select1
         /
  old  --
  */

  if (!isa<SelectInst>(I)) {
    return false;
  }

  llvm::Value *Mask, *New, *Old, *Select1;
  ICmpInst::Predicate Pred;

  // clang-format off
  if (!(match(&I, m_Select(
        m_ICmp(Pred, m_BitCast(m_Value(Mask)), m_SpecificInt(0)),
        m_Value(Select1),
        m_Value(Old))) && Pred == ICmpInst::ICMP_NE) &&
      !(match(&I, m_Select(
        m_ICmp(Pred, m_BitCast(m_Value(Mask)), m_SpecificInt(0)),
        m_Value(Old),
        m_Value(Select1))) && Pred == ICmpInst::ICMP_EQ)
     ) {
    return false;
  }
  // clang-format on

  assert(Mask && Old);
  if (!match(Select1,
             m_Select(m_Specific(Mask), m_Value(New), m_Specific(Old)))) {
    return false;
  }

  llvm::dbgs() << "replace:\n  ";
  I.print(llvm::dbgs());
  llvm::dbgs() << "\nwith:\n  ";
  Select1->print(llvm::dbgs());
  llvm::dbgs() << "\n";

  I.replaceAllUsesWith(Select1);
  return true;
}

bool DRAIRemoveDupSelect::RemoveSimdBranchSelect2(Instruction &I) {
  /*
replacing:
  mask ---> bitcast --> ne 0 -
        \                     \
         ----------------------> select
                              /
  zero -----------------------

with:
  mask
  */

  if (!isa<SelectInst>(I)) {
    return false;
  }

  llvm::Value *Mask;
  ICmpInst::Predicate Pred;

  // clang-format off
  if (!(match(&I, m_Select(
        m_ICmp(Pred, m_BitCast(m_Value(Mask)), m_SpecificInt(0)),
        m_Specific(Mask),
        m_Zero())) && Pred == ICmpInst::ICMP_NE) &&
      !(match(&I, m_Select(
        m_ICmp(Pred, m_BitCast(m_Value(Mask)), m_SpecificInt(0)),
        m_Zero(),
        m_Specific(Mask))) && Pred == ICmpInst::ICMP_EQ)
     ) {
    return false;
  }
  // clang-format on

  llvm::dbgs() << "replace:\n  ";
  I.print(llvm::dbgs());
  llvm::dbgs() << "\nwith:\n  ";
  Mask->print(llvm::dbgs());
  llvm::dbgs() << "\n";

  I.replaceAllUsesWith(Mask);
  return true;
}

} // namespace

INITIALIZE_PASS(DRAIRemoveDupSelect, DEBUG_TYPE,
                "DRAI Codegen Prepare Function Pass", false, false)

char DRAIRemoveDupSelect::ID = 0;
FunctionPass *llvm::createDRAIRemoveDupSelectPass() {
  return new DRAIRemoveDupSelect();
}
