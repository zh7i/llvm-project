#include "DRAISubtarget.h"
#include "DRAI.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"

namespace llvm {
cl::opt<bool>
    drai_hardware_loops("drai-hardware-loops", cl::Hidden,
                        cl::desc("for target DRAI, generate hardware loops"));
}

using namespace llvm;

#define DEBUG_TYPE "drai-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "DRAIGenSubtargetInfo.inc"

void DRAISubtarget::anchor() {}

DRAISubtarget::DRAISubtarget(const Triple &TT, const std::string &CPU,
                             const std::string &FS, const TargetMachine &TM)
    : DRAIGenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS), TargetTriple(TT),
      InstrInfo(*this), TLInfo(TM, *this), FrameLowering() {}

uint32_t DRAISubtarget::GetDRAISimdWidth() const { return 64; }

EVT DRAISubtarget::Vec(EVT e) const {
  if (e.isSimple()) {
    return Vec(e.getSimpleVT());
  }
  llvm_unreachable("only support simple type cvt to vector type");
  return EVT();
}

MVT DRAISubtarget::Vec(MVT e) const {
  return e.getVectorVT(e, GetDRAISimdWidth());
}

MVT::SimpleValueType DRAISubtarget::Vec(MVT::SimpleValueType e) const {
  return Vec(MVT(e)).SimpleTy;
}