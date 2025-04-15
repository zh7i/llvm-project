#include "DRAI.h"
#include "DRAIInstrInfo.h"
#include "DRAIPasses.h"
#include "DRAITargetMachine.h"

#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/PassRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "drai-codegen-prepare"

namespace {

class DRAICodeGenPrepare : public FunctionPass {
public:
  static char ID;

  DRAICodeGenPrepare() : FunctionPass(ID) {
    initializeDRAICodeGenPreparePass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override;

  StringRef getPassName() const override {
    return "DRAI Codegen Prepare Function Pass";
  }

private:
  bool OptimizeBlock(BasicBlock &BB);
  bool OptimizeInst(Instruction &I);

  // similar to llvm/lib/CodeGen/CodeGenPrepare.cpp sinkCmpExpression()
  bool SinkPredCondition(Instruction *I);
  bool SinkMaskedGatherScatter(Instruction *I);
};

bool DRAICodeGenPrepare::runOnFunction(Function &F) {
  llvm::dbgs() << "DRAICodeGenPrepare::runOnFunction hit!!!\n";

  bool changed = false;

  for (BasicBlock &BB : llvm::make_early_inc_range(F)) {
    changed |= OptimizeBlock(BB);
  }

  return changed;
}

bool DRAICodeGenPrepare::OptimizeBlock(BasicBlock &BB) {
  bool changed = false;

  for (Instruction &I : llvm::make_early_inc_range(BB)) {
    changed |= OptimizeInst(I);
  }

  return changed;
}

bool DRAICodeGenPrepare::OptimizeInst(Instruction &I) {
  bool change = false;

  change |= SinkPredCondition(&I);
  change |= SinkMaskedGatherScatter(&I);

  return change;
}

bool DRAICodeGenPrepare::SinkPredCondition(Instruction *I) {
  /*
  expect:              -- icmp --- condbr
                      /
  <64 x i1> - bitcast --- icmp --- condbr
                               \
                                -- condbr
  */

  // check
  if (!I || !isa<BranchInst>(I)) {
    return false;
  }
  BranchInst *BR = cast<BranchInst>(I);

  if (!BR->isConditional()) {
    return false;
  }
  Value *V = BR->getCondition();
  if (!V || !isa<ICmpInst>(V)) {
    return false;
  }
  ICmpInst *IC = cast<ICmpInst>(V);

  if (!IC) {
    return false;
  }
  unsigned ICIdx;
  if (isa<BitCastInst>(IC->getOperand(0))) {
    ICIdx = 0;
  } else if ((isa<BitCastInst>(IC->getOperand(1)))) {
    ICIdx = 1;
  } else {
    return false;
  }
  BitCastInst *BC = cast<BitCastInst>(IC->getOperand(ICIdx));

  if (!BC->getSrcTy()->isVectorTy() || !BC->getDestTy()->isIntegerTy()) {
    return false;
  }

  bool change = false;
  BasicBlock *BB = BR->getParent();

  // sink icmp -> condbr
  if (IC->getParent() != BB) {
    BasicBlock::iterator InsertPt = BB->getFirstInsertionPt();
    assert(IC->getOpcode() == Instruction::ICmp);
    IC = cast<ICmpInst>(ICmpInst::Create(IC->getOpcode(), IC->getPredicate(),
                                         IC->getOperand(0), IC->getOperand(1),
                                         "sinked", &*InsertPt));
    BR->setCondition(IC);
    change = true;
  }

  // sink bitcast -> icmp (maybe new)
  assert(IC->getParent() == BB);
  if (BC->getParent() != BB) {
    BasicBlock::iterator InsertPt = BB->getFirstInsertionPt();
    assert(BC->getOpcode() == Instruction::BitCast);
    BC = cast<BitCastInst>(
        BitCastInst::Create(BC->getOpcode(), BC->getOperand(0), BC->getDestTy(),
                            "sinked", &*InsertPt));
    IC->setOperand(ICIdx, BC);
    change = true;
  }

  return change;
}

bool DRAICodeGenPrepare::SinkMaskedGatherScatter(Instruction *I) {
  /*
  expect:            -- masked_gather
                   /
      ptr  ---- gep --- masked_scatter
               /
  <64 x i32> --
  */

  // check
  if (!I || !isa<IntrinsicInst>(I)) {
    return false;
  }
  IntrinsicInst *II = cast<IntrinsicInst>(I);
  unsigned PtrIdx = 0;
  if (II->getIntrinsicID() == Intrinsic::masked_gather) {
    PtrIdx = 0;
  } else if (II->getIntrinsicID() == Intrinsic::masked_scatter) {
    PtrIdx = 1;
  } else {
    return false;
  }

  GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(II->getOperand(PtrIdx));
  if (!GEP) {
    return false;
  }

  bool change = false;
  BasicBlock *BB = II->getParent();

  // sink gep -> masked gather/scatter
  if (GEP->getParent() != BB) {
    BasicBlock::iterator InsertPt = BB->getFirstInsertionPt();
    SmallVector<Value *, 8> GEPOps;
    for (Value *Op : GEP->operands()) {
      GEPOps.push_back(Op);
    }
    GEP = GetElementPtrInst::Create(GEP->getSourceElementType(), GEPOps[0],
                                    ArrayRef(GEPOps).slice(1), "sinked",
                                    &*InsertPt);
    II->setOperand(PtrIdx, GEP);
    change = true;
  }

  return change;
}
} // namespace

INITIALIZE_PASS(DRAICodeGenPrepare, DEBUG_TYPE,
                "DRAI Codegen Prepare Function Pass", false, false)

char DRAICodeGenPrepare::ID = 0;
FunctionPass *llvm::createDRAICodeGenPreparePass() {
  return new DRAICodeGenPrepare();
}
