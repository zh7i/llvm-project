def CC_DRAI : CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  CCIfSRet<CCIfType<[i32], CCAssignToReg<[PseudoR]>>>,

  // all register are loaded from the sram, %s0 be base address
  // CCIfType<[i32,i64,f32,f64], CCAssignToReg<[SR50, SR51, SR52, SR53, SR54, SR55]>>,

  CCAssignToStack<4, 4>
]>;

def RetCC_DRAI : CallingConv<[
  CCIfType<[i32,f32], CCAssignToReg<[PseudoR]>>,
  CCIfType<[i64,f64], CCAssignToReg<[PseudoR, PseudoR]>>,
  CCIfType<[vi32,vf32], CCAssignToReg<[PseudoR]>>
]>;

def CSR_DRAI : CalleeSavedRegs<(add)>;
def CSR_DRAIV : CalleeSavedRegs<(add)>;