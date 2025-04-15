add_llvm_component_library(LLVMDRAIAsmParser
  DRAIAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  DRAIDesc
  DRAIInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  DRAI
)