#include "DRAITargetMachine.h"
#include "DRAI.h"
#include "Passes/DRAIPasses.h"
//#include "DRAITargetObjectFile.h"
#include "DRAITargetTransformInfo.h"
#include "TargetInfo/DRAITargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDRAITarget() {
  // Register the target.
  RegisterTargetMachine<DRAITargetMachine> X(getTheDRAITarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeDRAILoopPseudoInstPass(PR);
  initializeDRAICodeGenPreparePass(PR);
  initializeDRAICvtHardwareLoopsPass(PR);
  initializeDRAIBlockReorderPass(PR);
  initializeDRAIMachineBlockReorderPass(PR);
  initializeDRAIRemoveDupSelectPass(PR);
}

namespace {
// TODO: Check.
std::string computeDataLayout(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string Ret;

  // little endian.
  Ret += "e";

  // Data mangling.
  Ret += DataLayout::getManglingComponent(TT);

  // Pointer's size, abi align, preferred align are 64 bit, Indexs are 32bit.
  // parse in DataLayout::parseSpecifier
  Ret += "-p:64:64:64:32";

  Ret += "-n32";

  // // Make sure that global data has at least 16 bits of alignment by
  // // default, so that we can refer to it using LARL.  We don't have any
  // // special requirements for stack variables though.
  // Ret += "-i1:8:16-i8:8:16";

  // // 64-bit integers are naturally aligned.
  // Ret += "-i64:64";

  // // 128-bit floats are aligned only to 64 bits.
  // Ret += "-f128:64";

  // // We prefer 16 bits of aligned for all globals; see above.
  // Ret += "-a:8:16";

  // // Integer registers are 32bits.
  // Ret += "-n32";

  return Ret;
}

// TODO: Check.
Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  if (!RM.has_value() || *RM == Reloc::DynamicNoPIC)
    return Reloc::Static;
  return *RM;
}

} // namespace

/// Create an DRAI architecture model
DRAITargetMachine::DRAITargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     std::optional<Reloc::Model> RM,
                                     std::optional<CodeModel::Model> CM,
                                     CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, FS), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Medium), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()) {
  initAsmInfo();
}

DRAITargetMachine::~DRAITargetMachine() {}

const DRAISubtarget *
DRAITargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<DRAISubtarget>(TargetTriple, CPU, FS, *this);
  }

  return I.get();
}

TargetTransformInfo
DRAITargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(DRAITTIImpl(this, F));
}

namespace {
/// DRAI Code Generator Pass Configuration Options.
class DRAIPassConfig : public TargetPassConfig {
public:
  DRAIPassConfig(DRAITargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  DRAITargetMachine &getDRAITargetMachine() const {
    return getTM<DRAITargetMachine>();
  }

  // look at their order here: addISelPasses, addMachinePasses
  bool addInstSelector() override;
  void addIRPasses() override;
  void addPreRegAlloc() override;
  void addPreEmitPass() override;
  void addBlockPlacement() override;
};
} // namespace

TargetPassConfig *DRAITargetMachine::createPassConfig(PassManagerBase &PM) {
  return new DRAIPassConfig(*this, PM);
}

bool DRAIPassConfig::addInstSelector() {
  addPass(createDRAIISelDag(getDRAITargetMachine(), getOptLevel()));
  return false;
}

void DRAIPassConfig::addPreRegAlloc() {
  // machine inst level, pre scheduling & hoisting
  addPass(createDRAILoopPseudoInstPass());
}

void DRAIPassConfig::addPreEmitPass() {
  // machine inst level, post register allocation
  // addPass(createDRAIRemoveReduentLoadPass());
}

void DRAIPassConfig::addIRPasses() {
  // llvm ir level, before create Selection DAG Builder
  addPass(createDRAIRemoveDupSelectPass());
  addPass(createInstructionCombiningPass());
  addPass(createEarlyCSEPass());
  addPass(createDeadCodeEliminationPass());

  // enable generic ir passes
  TargetPassConfig::addIRPasses();

  // hardware loops
  addPass(createHardwareLoopsLegacyPass());
  addPass(createDRAICvtHardwareLoopsPass());
  addPass(createEarlyCSEPass());
  addPass(createDeadCodeEliminationPass());

  // addPass(createDRAIRemoveReduentLoadPass());
  addPass(createDRAICodeGenPreparePass());
  addPass(createInferAddressSpacesPass());
  addPass(createDeadCodeEliminationPass());

  addPass(createDRAIBlockReorderPass());
}

void DRAIPassConfig::addBlockPlacement() {
  // replace and disable MachineBlockPlacement pass
  addPass(createDRAIMachineBlockReorderPass());
}