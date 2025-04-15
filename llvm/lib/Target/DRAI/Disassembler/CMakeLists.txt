add_llvm_component_library(LLVMDRAIDisassembler
  DRAIDisassembler.cpp

  LINK_COMPONENTS
  DRAIDesc
  DRAIInfo
  MCDisassembler
  Support

  ADD_TO_COMPONENT
  DRAI
)

