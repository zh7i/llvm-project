add_llvm_component_group(DRAI)

set(LLVM_TARGET_DEFINITIONS DRAI.td)

tablegen(LLVM DRAIGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM DRAIGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM DRAIGenCallingConv.inc -gen-callingconv)
tablegen(LLVM DRAIGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM DRAIGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM DRAIGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM DRAIGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM DRAIGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM DRAIGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(DRAICommonTableGen)

add_llvm_target(
  DRAICodeGen
  DRAIAsmPrinter.cpp
  DRAIFrameLowering.cpp
  DRAIISelDAGToDAG.cpp
  DRAIISelLowering.cpp
  DRAIInstrInfo.cpp
  DRAIMCInstLower.cpp
  DRAIRegisterInfo.cpp
  DRAISubtarget.cpp
  DRAITargetMachine.cpp
  DRAITargetTransformInfo.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  IPO
  MC
  DRAIDesc
  DRAIInfo
  Scalar
  SelectionDAG
  Support
  Target
  TransformUtils
  Vectorize
  DRAIPasses

  ADD_TO_COMPONENT
  DRAI
  )

add_subdirectory(Disassembler)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(AsmParser)
add_subdirectory(Passes)
