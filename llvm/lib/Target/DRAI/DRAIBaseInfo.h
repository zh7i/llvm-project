#ifndef LLVM_LIB_TARGET_DRAI_DRAIBASEINFO_H
#define LLVM_LIB_TARGET_DRAI_DRAIBASEINFO_H

namespace llvm {

enum class DRAIAddressSpace {
  ADDRSPACE_GLOBAL = 0,
  ADDRSPACE_SHARED = 1,
};

} // end namespace llvm
#endif