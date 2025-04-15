#include "DRAI.h"
#include "DRAIPasses.h"
#include "DRAIFrameLowering.h"
#include "DRAIInstrInfo.h"
//#include "DRAIMachineFunction.h"
#include "DRAISubtarget.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

#define DEBUG_TYPE "DRAI-remove-reduant-load"
namespace{
class DRAIRemoveReduantLoad : public FunctionPass {
public:
  static char ID;

  const DRAISubtarget *STI;
  const DRAIInstrInfo *TII;
  const DRAIRegisterInfo *TRI;
  //const DRAIMachineFunctionInfo *MFI;

  DRAIRemoveReduantLoad() : FunctionPass(ID) {}

  bool runOnFunction(Function &MF) override {
    // //STI = &MF.getSubtarget<DRAISubtarget>();
    // //TII = STI->getInstrInfo();
    // //TRI = STI->getRegisterInfo();
    // //MFI = MF.getInfo<DRAIMachineFunctionInfo>();
    // //FL = STI->getFrameLowering();
    //     // Argument arg;
    // // arg.users
    // SmallVector<Use *, 8> Uses;
    // // LLVMContext ctx = MF.begin()->getContext();
    // IRBuilder<> Builder(MF.begin()->getContext());
    // for(Argument &Arg : MF.args()){
    //     // Value *VecValue = Builder.CreateLoad(Arg.getParamByValType(), &Arg);
    //     // Arg.replaceAllUsesWith(VecValue);

    //     for(Use &U : Arg.uses()){
    //         Uses.push_back(&U);
    //         Use *UP = Uses.pop_back_val();
    //
    //         Instruction *TI = dyn_cast<Instruction>(UP->getUser());
    //         std::cout<<"\n\nTI.getOpcode() = " << TI->getOpcode() << std::endl;
    //     }
    // }
    // auto MRI = &MF.getRegInfo();
    // SmallVector<const MachineInstr*, 8> Users;

    // for (auto &MBB : MF) {
    //   //auto MI = MBB.begin(), E = MBB.end();
    //   for (MachineInstr &MI : MBB){
    //     switch (MI.getOpcode()){
    //       default:
    //         break;
    //       case DRAI::LoadS:
    //         MachineOperand &op0 = MI.getOperand(0);
    //         Register reg = op0.getReg();
    //         // for (const MachineInstr &I : MRI->use_nodbg_instructions(reg)){
    //         //     Users.push_back(&I);
    //         // }
    //         MachineRegisterInfo::def_instr_iterator Def =
    //             MRI->def_instr_begin(reg);
    //         // std::cout << "\nsize= " << Users.size() << std::endl;
    //         // const MachineInstr *I = Users.pop_back_val();
    //         std::cout<<"User code: " << (*&Def)->getOpcode() << ", LoadS= " << DRAI::LoadS <<std::endl;
    //         break;
    //     }
    //     //std::cout<<"\n\nI.getOpcode() = " << I.getOpcode() << std::endl;

    //     //if(I.getOpcode() == Instruction::Load)
    //   }
    //   while (MI != E) {
    //     // Processing might change current instruction, save next first
    //     auto NMI = std::next(MI);
    //     switch (MI->getOpcode()) {
    //     default:
    //       break;
    //     case DRAI::LoadS:
    //       MachineBasicBlock::instr_iterator BundledMI = MI->getIterator();
    //       MachineInstr &MInst = *MI++;
    //       MachineInstr &NextInstr = *(++BundledMI);
    //     //   auto NextOpcode = NextInstr.getOpcode();
    //     //   if((NextOpcode == DRAI::LoadS) | (NextOpcode == DRAI::StoreS)){
    //     //     MInst.eraseFromParent();
    //     //   }
    //       //ReplaceNode(N, CurDAG->getMachineNode(MLAOpc, dl, N->getValueType(0), Ops));
    //       //MInst.eraseFromParent();
    //       //getNumOperands

    //       std::cout << "\n\nOpcode is: " << NextInstr.getOperand(0).getReg()<< std::endl;
    //       break;
    //     }

    //     MI = NMI;
    //   }
    // }
    return true;
  }

  StringRef getPassName() const override { return "DRAI remove reduant load pass"; }
};
char DRAIRemoveReduantLoad::ID = 0;
}

// /// Returns an instance of the pseudo instruction expansion pass.
// FunctionPass *llvm::createDRAIRemoveReduentLoadPass() {
//   return new DRAIRemoveReduantLoad();
// }
