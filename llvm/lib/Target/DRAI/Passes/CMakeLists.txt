add_llvm_component_library(LLVMDRAIPasses
  DRAICodeGenPreparePass.cpp
  DRAILoopPseudoInstPass.cpp
  DRAIRemoveReduantLoadPass.cpp
  DRAICvtHardwareLoopsPass.cpp
  DRAIBlockReorderPass.cpp
  DRAIRemoveDupSelectPass.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  DRAI
  )
