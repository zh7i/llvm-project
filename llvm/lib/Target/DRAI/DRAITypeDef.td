class CmdTypeEnumValue<int val_> {
  int val = val_;
}
class CmdTypeEnumClass {
  CmdTypeEnumValue None           = CmdTypeEnumValue<0>;
  CmdTypeEnumValue Index          = CmdTypeEnumValue<10>;
  CmdTypeEnumValue Gather_Fast    = CmdTypeEnumValue<20>;
  CmdTypeEnumValue Scatter_Fast   = CmdTypeEnumValue<21>;
  CmdTypeEnumValue Gather_Slow    = CmdTypeEnumValue<22>;
  CmdTypeEnumValue Scatter_Slow   = CmdTypeEnumValue<23>;
  CmdTypeEnumValue Load_Fast      = CmdTypeEnumValue<24>;
  CmdTypeEnumValue Store_Fast     = CmdTypeEnumValue<25>;
  CmdTypeEnumValue Load_Slow      = CmdTypeEnumValue<26>;
  CmdTypeEnumValue Store_Slow     = CmdTypeEnumValue<27>;
  CmdTypeEnumValue Compare_V      = CmdTypeEnumValue<30>;
  CmdTypeEnumValue Arithmetic_V   = CmdTypeEnumValue<40>;
  CmdTypeEnumValue Conv2D         = CmdTypeEnumValue<50>;
  CmdTypeEnumValue Rand           = CmdTypeEnumValue<51>;
  CmdTypeEnumValue Cast_V         = CmdTypeEnumValue<52>;
  CmdTypeEnumValue Counter        = CmdTypeEnumValue<53>;
  CmdTypeEnumValue Arith_Reduce   = CmdTypeEnumValue<60>;
  CmdTypeEnumValue Shift_Reduce   = CmdTypeEnumValue<61>;
  CmdTypeEnumValue Sort_Reduce    = CmdTypeEnumValue<62>;
  CmdTypeEnumValue Reverse_Reduce = CmdTypeEnumValue<63>;
  CmdTypeEnumValue Select_Reduce  = CmdTypeEnumValue<64>;
  CmdTypeEnumValue Arithmetic_S   = CmdTypeEnumValue<70>;
  CmdTypeEnumValue Compare_S      = CmdTypeEnumValue<71>;
  CmdTypeEnumValue Cast_S         = CmdTypeEnumValue<72>;
  CmdTypeEnumValue Arithmetic_P   = CmdTypeEnumValue<80>;
  CmdTypeEnumValue DMA            = CmdTypeEnumValue<100>;
  CmdTypeEnumValue Stride_Load    = CmdTypeEnumValue<101>;
  CmdTypeEnumValue Stride_Store   = CmdTypeEnumValue<102>;
  CmdTypeEnumValue Move           = CmdTypeEnumValue<110>;
  CmdTypeEnumValue Move_I         = CmdTypeEnumValue<111>;
  CmdTypeEnumValue Select         = CmdTypeEnumValue<112>;
  CmdTypeEnumValue Jmp            = CmdTypeEnumValue<200>;
  CmdTypeEnumValue Loop           = CmdTypeEnumValue<201>;
  CmdTypeEnumValue Nop            = CmdTypeEnumValue<220>;
  CmdTypeEnumValue Cmodel         = CmdTypeEnumValue<230>;
  CmdTypeEnumValue MemoryFence    = CmdTypeEnumValue<240>;
  CmdTypeEnumValue SyncThreads    = CmdTypeEnumValue<241>;
  CmdTypeEnumValue Ret            = CmdTypeEnumValue<255>;
}
def CmdType : CmdTypeEnumClass;

class DataTypeEnumValue<int val_, string asm_, ValueType vt_s_, ValueType vt_v_, RegisterOperand opnd_s_> {
  int val = val_;
  string asm = asm_;
  ValueType vt_s = vt_s_;
  ValueType vt_v = vt_v_;
  RegisterOperand opnd_s = opnd_s_;
}
class DataTypeEnumClass {
  DataTypeEnumValue None  = DataTypeEnumValue<0, "", OtherVT, OtherVT, PseudoOpnd>;
  DataTypeEnumValue Fp32  = DataTypeEnumValue<1, ".f32", f32, vf32, SRegOpnd>;
  DataTypeEnumValue Int32 = DataTypeEnumValue<2, ".i32", i32, vi32, SRegOpnd>;
  DataTypeEnumValue Fp16  = DataTypeEnumValue<3, ".f16", f16, vf16, SRegOpnd>;
  DataTypeEnumValue Int64 = DataTypeEnumValue<4, ".i64", i64, vi64, ERegOpnd>;
}
def DataType : DataTypeEnumClass;

class CompareOpEnumValue<int val_> {
  int val = val_;
}
class CompareOpEnumClass {
  CompareOpEnumValue NONE = CompareOpEnumValue<0>;
  CompareOpEnumValue EQ   = CompareOpEnumValue<1>;
  CompareOpEnumValue NE   = CompareOpEnumValue<2>;
  CompareOpEnumValue LT   = CompareOpEnumValue<3>;
  CompareOpEnumValue LE   = CompareOpEnumValue<4>;
  CompareOpEnumValue GT   = CompareOpEnumValue<5>;
  CompareOpEnumValue GE   = CompareOpEnumValue<6>;
}
def CompareOp : CompareOpEnumClass;

class ArithmeticOpEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class ArithmeticOpEnumClass {
  ArithmeticOpEnumValue NONE  = ArithmeticOpEnumValue<0, "">;
  ArithmeticOpEnumValue ADD   = ArithmeticOpEnumValue<1, ".add">;
  ArithmeticOpEnumValue SUB   = ArithmeticOpEnumValue<2, ".sub">;
  ArithmeticOpEnumValue MUL   = ArithmeticOpEnumValue<3, ".mul">;
  ArithmeticOpEnumValue DIV   = ArithmeticOpEnumValue<4, ".div">;
  ArithmeticOpEnumValue REM   = ArithmeticOpEnumValue<5, ".rem">;
  ArithmeticOpEnumValue ABS   = ArithmeticOpEnumValue<6, ".abs">;
  ArithmeticOpEnumValue MAX   = ArithmeticOpEnumValue<7, ".max">;
  ArithmeticOpEnumValue MIN   = ArithmeticOpEnumValue<8, ".min">;
  ArithmeticOpEnumValue SQRT  = ArithmeticOpEnumValue<9, ".sqrt">;
  ArithmeticOpEnumValue SIN   = ArithmeticOpEnumValue<10, ".sin">;
  ArithmeticOpEnumValue COS   = ArithmeticOpEnumValue<11, ".cos">;
  ArithmeticOpEnumValue TAN   = ArithmeticOpEnumValue<12, ".tan">;
  ArithmeticOpEnumValue LOG2  = ArithmeticOpEnumValue<13, ".log2">;
  ArithmeticOpEnumValue EXP2  = ArithmeticOpEnumValue<14, ".exp2">;
  ArithmeticOpEnumValue LOG   = ArithmeticOpEnumValue<15, ".log">;
  ArithmeticOpEnumValue EXP   = ArithmeticOpEnumValue<16, ".exp">;
  ArithmeticOpEnumValue LOG10 = ArithmeticOpEnumValue<17, ".log10">;
  ArithmeticOpEnumValue EXP10 = ArithmeticOpEnumValue<18, ".exp10">;
  ArithmeticOpEnumValue AND   = ArithmeticOpEnumValue<40, ".and">;
  ArithmeticOpEnumValue OR    = ArithmeticOpEnumValue<41, ".or">;
  ArithmeticOpEnumValue XOR   = ArithmeticOpEnumValue<42, ".xor">;
  ArithmeticOpEnumValue NOT   = ArithmeticOpEnumValue<43, ".not">;
  ArithmeticOpEnumValue SHL   = ArithmeticOpEnumValue<44, ".shl">;
  ArithmeticOpEnumValue SHR   = ArithmeticOpEnumValue<45, ".shr">;
}
def ArithmeticOp : ArithmeticOpEnumClass;

class RegRegTypeEnumValue<int val_> {
  int val = val_;
}
class RegRegTypeEnumClass {
  RegRegTypeEnumValue None = RegRegTypeEnumValue<0>;
  RegRegTypeEnumValue VV   = RegRegTypeEnumValue<1>;
  RegRegTypeEnumValue VS   = RegRegTypeEnumValue<2>;
  RegRegTypeEnumValue SV   = RegRegTypeEnumValue<3>;
  RegRegTypeEnumValue SS   = RegRegTypeEnumValue<4>;
  RegRegTypeEnumValue PP   = RegRegTypeEnumValue<5>;
}
def RegRegType : RegRegTypeEnumClass;

class RandTypeEnumValue<int val_> {
  int val = val_;
}
class RandTypeEnumClass {
  RandTypeEnumValue None    = RandTypeEnumValue<0>;
  RandTypeEnumValue Uni_Int = RandTypeEnumValue<1>;
}
def RandType : RandTypeEnumClass;

class CastTypeEnumValue<int val_, string asm_, ValueType src_vt_s_, ValueType src_vt_v_, ValueType dst_vt_s_, ValueType dst_vt_v_, RegisterOperand src_opnd_s_, RegisterOperand dst_opnd_s_> {
  int val = val_;
  string asm = asm_;
  ValueType src_vt_s = src_vt_s_;
  ValueType src_vt_v = src_vt_v_;
  ValueType dst_vt_s = dst_vt_s_;
  ValueType dst_vt_v = dst_vt_v_;
  RegisterOperand src_opnd_s = src_opnd_s_;
  RegisterOperand dst_opnd_s = dst_opnd_s_;
}
class CastTypeEnumClass {
  CastTypeEnumValue None                 = CastTypeEnumValue<0, "", OtherVT, OtherVT, OtherVT, OtherVT, PseudoOpnd, PseudoOpnd>;
  CastTypeEnumValue Int32_To_Fp32        = CastTypeEnumValue<1, ".f32.i32", i32, vi32, f32, vf32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp32_To_Int32_Round  = CastTypeEnumValue<2, ".rn.i32.f32", f32, vf32, i32, vi32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp32_To_Int32_Ceil   = CastTypeEnumValue<3, ".rc.i32.f32", f32, vf32, i32, vi32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp32_To_Int32_Floor  = CastTypeEnumValue<4, ".rf.i32.f32", f32, vf32, i32, vi32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp32_To_Int32_Zero   = CastTypeEnumValue<5, ".i32.f32", f32, vf32, i32, vi32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Int32_To_Int64_Sign  = CastTypeEnumValue<6, ".i64.i32", i32, OtherVT, i64, OtherVT, SRegOpnd, ERegOpnd>;
  CastTypeEnumValue Fp32_To_Fp16         = CastTypeEnumValue<7, ".f16.f32", f32, vf32, f16, vf16, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp16_To_Fp32         = CastTypeEnumValue<8, ".f32.f16", f16, vf16, f32, vf32, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Int32_To_Fp16        = CastTypeEnumValue<9, ".f16.i32", i32, vi32, f16, vf16, SRegOpnd, SRegOpnd>;
  CastTypeEnumValue Fp16_To_Int32_Zero   = CastTypeEnumValue<10, ".i32.f16", f16, vf16, i32, vi32, SRegOpnd, SRegOpnd>;
}
def CastType : CastTypeEnumClass;

class PredStateTypeEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class PredStateTypeEnumClass {
  PredStateTypeEnumValue None        = PredStateTypeEnumValue<0, "">;
  PredStateTypeEnumValue State_None  = PredStateTypeEnumValue<1, ".none">;
  PredStateTypeEnumValue State_First = PredStateTypeEnumValue<2, ".first">;
  PredStateTypeEnumValue State_Any   = PredStateTypeEnumValue<3, ".any">;
  PredStateTypeEnumValue State_All   = PredStateTypeEnumValue<4, ".all">;
}
def PredStateType : PredStateTypeEnumClass;

class MoveTypeEnumValue<int val_, string asm_, RegisterOperand src_opnd_, RegisterOperand dst_opnd_> {
  int val = val_;
  string asm = asm_;
  RegisterOperand src_opnd = src_opnd_;
  RegisterOperand dst_opnd = dst_opnd_;
}
class MoveTypeEnumClass {
  MoveTypeEnumValue NONE                     = MoveTypeEnumValue<0, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue DRAM_TO_DRAM             = MoveTypeEnumValue<1, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue DRAM_TO_SRAM             = MoveTypeEnumValue<2, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue SRAM_TO_DRAM             = MoveTypeEnumValue<3, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue SRAM_TO_SRAM             = MoveTypeEnumValue<4, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue ZERO_TO_DRAM             = MoveTypeEnumValue<5, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue ZERO_TO_SRAM             = MoveTypeEnumValue<6, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FROM_TO_VECTOR           = MoveTypeEnumValue<1, ".v", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FROM_TO_SCALAR           = MoveTypeEnumValue<2, ".s", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue DRAM_TO_FSRAM_HORIZONTAL = MoveTypeEnumValue<1, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue DRAM_TO_FSRAM_VERTICAL   = MoveTypeEnumValue<2, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue SRAM_TO_FSRAM_HORIZONTAL = MoveTypeEnumValue<3, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue SRAM_TO_FSRAM_VERTICAL   = MoveTypeEnumValue<4, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FSRAM_TO_DRAM_HORIZONTAL = MoveTypeEnumValue<1, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FSRAM_TO_DRAM_VERTICAL   = MoveTypeEnumValue<2, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FSRAM_TO_SRAM_HORIZONTAL = MoveTypeEnumValue<3, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue FSRAM_TO_SRAM_VERTICAL   = MoveTypeEnumValue<4, "", PseudoOpnd, PseudoOpnd>;
  MoveTypeEnumValue VEC_REG_TO_VEC_REG       = MoveTypeEnumValue<1, ".vv", VRegOpnd, VRegOpnd>;
  MoveTypeEnumValue ZERO_TO_VEC_REG          = MoveTypeEnumValue<2, ".vi", PseudoOpnd, VRegOpnd>;
  MoveTypeEnumValue SCALAR_REG_TO_SCALAR_REG = MoveTypeEnumValue<3, ".ss", SRegOpnd, SRegOpnd>;
  MoveTypeEnumValue PRED_REG_TO_PRED_REG     = MoveTypeEnumValue<4, ".pp", PRegOpnd, PRegOpnd>;
  MoveTypeEnumValue ONE_TO_PRED_REG          = MoveTypeEnumValue<5, ".pi", PseudoOpnd, PRegOpnd>;
  MoveTypeEnumValue SCALAR_REG_TO_VEC_REG    = MoveTypeEnumValue<6, ".vs", SRegOpnd, VRegOpnd>;
  MoveTypeEnumValue S64_REG_TO_S64_REG       = MoveTypeEnumValue<7, ".ee", ERegOpnd, ERegOpnd>;
  MoveTypeEnumValue CONST_TO_S64_REG         = MoveTypeEnumValue<8, ".ei", PseudoOpnd, ERegOpnd>;
  MoveTypeEnumValue SPECIAL_TO_SCALAR_REG    = MoveTypeEnumValue<13, ".sys", PseudoOpnd, SRegOpnd>;
  MoveTypeEnumValue SCALAR_TO_SPECIAL_REG    = MoveTypeEnumValue<14, ".sys", ERegOpnd, PseudoOpnd>;
  MoveTypeEnumValue CONST_TO_SCALAR_REG      = MoveTypeEnumValue<15, ".si", PseudoOpnd, SRegOpnd>;
}
def MoveType : MoveTypeEnumClass;

class ShiftReduceTypeEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class ShiftReduceTypeEnumClass {
  ShiftReduceTypeEnumValue None           = ShiftReduceTypeEnumValue<0, "">;
  ShiftReduceTypeEnumValue Circular_Left  = ShiftReduceTypeEnumValue<1, ".l">;
  ShiftReduceTypeEnumValue Circular_Right = ShiftReduceTypeEnumValue<2, ".r">;
}
def ShiftReduceType : ShiftReduceTypeEnumClass;

class SortTypeEnumValue<int val_> {
  int val = val_;
}
class SortTypeEnumClass {
  SortTypeEnumValue None           = SortTypeEnumValue<0>;
  SortTypeEnumValue Ascending      = SortTypeEnumValue<1>;
  SortTypeEnumValue Descending     = SortTypeEnumValue<2>;
  SortTypeEnumValue AscendingAndC  = SortTypeEnumValue<3>;
  SortTypeEnumValue DescendingAndC = SortTypeEnumValue<4>;
}
def SortType : SortTypeEnumClass;

// class ScatterFeatureTypeEnumValue<int val_> {
//   int val = val_;
// }
// class ScatterFeatureTypeEnumClass {
//   ScatterFeatureTypeEnumValue None            = ScatterFeatureTypeEnumValue<0>;
//   ScatterFeatureTypeEnumValue OrderedDiscrete = ScatterFeatureTypeEnumValue<1>;
//   ScatterFeatureTypeEnumValue OrderedDense    = ScatterFeatureTypeEnumValue<2>;
//   ScatterFeatureTypeEnumValue UnorderDiscrete = ScatterFeatureTypeEnumValue<3>;
//   ScatterFeatureTypeEnumValue UnorderDense    = ScatterFeatureTypeEnumValue<4>;
// }
// def ScatterFeatureType : ScatterFeatureTypeEnumClass;

class RegisterTypeEnumValue<int val_, string asm_, RegisterOperand opnd_> {
  int val = val_;
  string asm = asm_;
  RegisterOperand opnd = opnd_;
}
class RegisterTypeEnumClass {
  RegisterTypeEnumValue None      = RegisterTypeEnumValue<0, "", PseudoOpnd>;
  RegisterTypeEnumValue Vector    = RegisterTypeEnumValue<1, ".v", VRegOpnd>;
  RegisterTypeEnumValue Scalar    = RegisterTypeEnumValue<2, ".s", SRegOpnd>;
  RegisterTypeEnumValue Pred      = RegisterTypeEnumValue<3, ".p", PRegOpnd>;
  RegisterTypeEnumValue EScalar   = RegisterTypeEnumValue<4, ".e", ERegOpnd>;
}
def RegisterType : RegisterTypeEnumClass;

class CModelOpTypeEnumValue<int val_> {
  int val = val_;
}
class CModelOpTypeEnumClass {
  CModelOpTypeEnumValue None     = CModelOpTypeEnumValue<0>;
  CModelOpTypeEnumValue PrintReg = CModelOpTypeEnumValue<1>;
  CModelOpTypeEnumValue PrintStr = CModelOpTypeEnumValue<2>;
  CModelOpTypeEnumValue Assert   = CModelOpTypeEnumValue<3>;
}
def CModelOpType : CModelOpTypeEnumClass;

class AddressSpaceEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class AddressSpaceEnumClass {
  AddressSpaceEnumValue Global = AddressSpaceEnumValue<0, "">;
  AddressSpaceEnumValue Shared = AddressSpaceEnumValue<1, ".sm">;
}
def AddressSpace : AddressSpaceEnumClass;

class SyncScopeEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class SyncScopeEnumClass {
  SyncScopeEnumValue None = SyncScopeEnumValue<0, "">;
}
def SyncScope : SyncScopeEnumClass;

class FenceScopeEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class FenceScopeEnumClass {
  FenceScopeEnumValue None = FenceScopeEnumValue<0, "">;
}
def FenceScope : FenceScopeEnumClass;

class SystemRegisterEnumValue<int val_, string asm_> {
  int val = val_;
  string asm = asm_;
}
class SystemRegisterEnumClass {
  SystemRegisterEnumValue None    = SystemRegisterEnumValue<0, "">;
  SystemRegisterEnumValue CoreDim = SystemRegisterEnumValue<0xa, "coredim">;
  SystemRegisterEnumValue CoreIdx = SystemRegisterEnumValue<0x8, "coreidx">;
}
def SystemRegister : SystemRegisterEnumClass;
