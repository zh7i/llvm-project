add_llvm_component_library(LLVMDRAIInfo
  DRAITargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  DRAI
  )
