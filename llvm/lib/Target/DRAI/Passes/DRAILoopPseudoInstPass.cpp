#include "DRAI.h"
#include "DRAIPasses.h"
#include "DRAIInstrInfo.h"
#include "DRAITargetMachine.h"

#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/PassRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "drai-loop-mfp"

namespace {

class DRAILoopPseudoInst : public MachineFunctionPass {
public:
  const DRAIInstrInfo *TII;
  const DRAIRegisterInfo *TRI;
  static char ID;

  DRAILoopPseudoInst() : MachineFunctionPass(ID) {
    initializeDRAILoopPseudoInstPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return "DRAI Loop Pseudo Inst Machine Function Pass";
  }

private:
  bool LoopPseudoMoveToFirstTerminator(MachineInstr &MI);
};

bool DRAILoopPseudoInst::runOnMachineFunction(MachineFunction &MF) {
  bool changed = false;

  for (MachineBasicBlock &B : MF) {
    for (MachineInstr &MI : B) {
      if (MI.getOpcode() == DRAI::LoopPseudoSetIter) {
        changed |= LoopPseudoMoveToFirstTerminator(MI);
      }
    }
  }

  return changed;
}

bool DRAILoopPseudoInst::LoopPseudoMoveToFirstTerminator(MachineInstr &MI) {
  if (!MI.isTerminator()) {
    llvm_unreachable("LoopPseudoSetIter must is terminator!!");
  }
  MachineBasicBlock *MBB = MI.getParent();
  MachineInstr *set_iter = &MI;

  if (MBB->getFirstInstrTerminator() != MBB->end()) { // has terminator
    MachineInstr *ter = &(*MBB->getFirstInstrTerminator());
    if (ter == &MI) {
      return false; // already it is
    }

    set_iter->moveBefore(ter);
  } else {                        // optimized in SelectionDAGBuilder::visitBr
    set_iter->removeFromParent(); // remove from MBB, but not destory
    MBB->push_back(set_iter);
  }

  return true;
}

} // namespace

INITIALIZE_PASS(DRAILoopPseudoInst, DEBUG_TYPE,
                "DRAI Loop Pseudo Inst Machine Function Pass", false, false)

char DRAILoopPseudoInst::ID = 0;
FunctionPass *llvm::createDRAILoopPseudoInstPass() {
  return new DRAILoopPseudoInst();
}
