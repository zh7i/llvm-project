// Index
def Index : InstSIMDIndex;

foreach addr_space = [AddressSpace.Global, AddressSpace.Shared] in { // for memory instructions

// Gather
defm Gather : SIMDGather<"gather", DataType.Int32, addr_space>;
defm Gather : SIMDGather<"gather", DataType.Fp32, addr_space>;
defm Gather : SIMDGather<"gather", DataType.Fp16, addr_space>;

// Scatter
defm Scatter : SIMDScatter<"scatter", DataType.Int32, addr_space>;
defm Scatter : SIMDScatter<"scatter", DataType.Fp32, addr_space>;
defm Scatter : SIMDScatter<"scatter", DataType.Fp16, addr_space>;

defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_i, DataType.Int32, ArithmeticOp.ADD, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_i, DataType.Int32, ArithmeticOp.MAX, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_i, DataType.Int32, ArithmeticOp.MIN, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_i, DataType.Int32, ArithmeticOp.AND, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_i, DataType.Int32, ArithmeticOp.OR, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_f, DataType.Fp32, ArithmeticOp.ADD, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_f, DataType.Fp32, ArithmeticOp.MAX, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_f, DataType.Fp32, ArithmeticOp.MIN, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_h, DataType.Fp16, ArithmeticOp.ADD, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_h, DataType.Fp16, ArithmeticOp.MAX, addr_space>;
defm Scatter : SIMDScatterOper<"scatter", int_drai_scatter_store_h, DataType.Fp16, ArithmeticOp.MIN, addr_space>;

// Load
defm LoadS : LoadScalar<"load", DataType.Int32, MoveType.FROM_TO_SCALAR, addr_space>;
defm LoadS : LoadScalar<"load", DataType.Fp16, MoveType.FROM_TO_SCALAR, addr_space>;
defm LoadS : LoadScalar<"load", DataType.Fp32, MoveType.FROM_TO_SCALAR, addr_space>;

defm LoadV : LoadVector<"load", DataType.Int32, MoveType.FROM_TO_VECTOR, addr_space>;
defm LoadV : LoadVector<"load", DataType.Fp16, MoveType.FROM_TO_VECTOR, addr_space>;
defm LoadV : LoadVector<"load", DataType.Fp32, MoveType.FROM_TO_VECTOR, addr_space>;

// Store
defm StoreS : StoreScalar<"store", DataType.Int32, MoveType.FROM_TO_SCALAR, addr_space>;
defm StoreS : StoreScalar<"store", DataType.Fp16, MoveType.FROM_TO_SCALAR, addr_space>;
defm StoreS : StoreScalar<"store", DataType.Fp32, MoveType.FROM_TO_SCALAR, addr_space>;

defm StoreV : StoreVector<"store", DataType.Int32, MoveType.FROM_TO_VECTOR, addr_space>;
defm StoreV : StoreVector<"store", DataType.Fp16, MoveType.FROM_TO_VECTOR, addr_space>;
defm StoreV : StoreVector<"store", DataType.Fp32, MoveType.FROM_TO_VECTOR, addr_space>;

}

// 64bit Load
defm LoadE : LoadScalar<"load", DataType.Int64, MoveType.FROM_TO_SCALAR, AddressSpace.Global>;

// ArithScalar
foreach data_type = [DataType.Int32] in {
  defm ArithS : ArithScalar<"add", add,  data_type, ArithmeticOp.ADD>;
  defm ArithS : ArithScalar<"sub", sub,  data_type, ArithmeticOp.SUB>;
  defm ArithS : ArithScalar<"mul", mul,  data_type, ArithmeticOp.MUL>;
  defm ArithS : ArithScalar<"div", sdiv, data_type, ArithmeticOp.DIV>;
  defm ArithS : ArithScalar<"rem", srem, data_type, ArithmeticOp.REM>;
  defm ArithS : ArithScalar<"and", and,  data_type, ArithmeticOp.AND>;
  defm ArithS : ArithScalar<"or",  or,   data_type, ArithmeticOp.OR>;
  defm ArithS : ArithScalar<"xor", xor,  data_type, ArithmeticOp.XOR>;
  defm ArithS : ArithScalar<"shl", shl,  data_type, ArithmeticOp.SHL>;
  defm ArithS : ArithScalar<"shr", sra,  data_type, ArithmeticOp.SHR>; // arithmetic shift right
}
foreach data_type = [DataType.Fp16, DataType.Fp32] in {
  defm ArithS : ArithScalar<"add", fadd, data_type, ArithmeticOp.ADD>;
  defm ArithS : ArithScalar<"sub", fsub, data_type, ArithmeticOp.SUB>;
  defm ArithS : ArithScalar<"mul", fmul, data_type, ArithmeticOp.MUL>;
  defm ArithS : ArithScalar<"div", fdiv, data_type, ArithmeticOp.DIV>;
  defm ArithS : ArithUnaryScalar<"sqrt", fsqrt, data_type, ArithmeticOp.SQRT>;
  defm ArithS : ArithUnaryScalar<"sin", fsin, data_type, ArithmeticOp.SIN>;
  defm ArithS : ArithUnaryScalar<"cos", fcos, data_type, ArithmeticOp.COS>;
  defm ArithS : ArithUnaryScalar<"exp", fexp, data_type, ArithmeticOp.EXP>;
  defm ArithS : ArithUnaryScalar<"exp2", fexp2, data_type, ArithmeticOp.EXP2>;
  defm ArithS : ArithUnaryScalar<"log", flog, data_type, ArithmeticOp.LOG>;
  defm ArithS : ArithUnaryScalar<"log10", flog10, data_type, ArithmeticOp.LOG10>;
  defm ArithS : ArithUnaryScalar<"log2", flog2, data_type, ArithmeticOp.LOG2>;
}

// 64bit
defm ArithE : ArithScalar<"add", add,  DataType.Int64, ArithmeticOp.ADD>;
defm ArithE : ArithScalar<"sub", sub,  DataType.Int64, ArithmeticOp.SUB>;
defm ArithE : ArithScalar<"mul", mul,  DataType.Int64, ArithmeticOp.MUL>;

// ArithVector
foreach data_type = [DataType.Int32] in {
  defm ArithV : ArithVector<"add.v", add,  data_type, ArithmeticOp.ADD>;
  defm ArithV : ArithVector<"sub.v", sub,  data_type, ArithmeticOp.SUB>;
  defm ArithV : ArithVector<"mul.v", mul,  data_type, ArithmeticOp.MUL>;
  defm ArithV : ArithVector<"div.v", sdiv, data_type, ArithmeticOp.DIV>;
  defm ArithV : ArithVector<"rem.v", srem, data_type, ArithmeticOp.REM>;
  defm ArithV : ArithVector<"and.v", and,  data_type, ArithmeticOp.AND>;
  defm ArithV : ArithVector<"or.v",  or,   data_type, ArithmeticOp.OR>;
  defm ArithV : ArithVector<"xor.v", xor,  data_type, ArithmeticOp.XOR>;
  defm ArithV : ArithVector<"shl.v", shl,  data_type, ArithmeticOp.SHL>;
  defm ArithV : ArithVector<"shr.v", sra,  data_type, ArithmeticOp.SHR>; // arithmetic shift right
}
foreach data_type = [DataType.Fp16, DataType.Fp32] in {
  defm ArithV : ArithVector<"add.v", fadd, data_type, ArithmeticOp.ADD>;
  defm ArithV : ArithVector<"sub.v", fsub, data_type, ArithmeticOp.SUB>;
  defm ArithV : ArithVector<"mul.v", fmul, data_type, ArithmeticOp.MUL>;
  defm ArithV : ArithVector<"div.v", fdiv, data_type, ArithmeticOp.DIV>;
  defm ArithV : ArithUnaryVector<"sqrt.v", fsqrt, data_type, ArithmeticOp.SQRT>;
  defm ArithV : ArithUnaryVector<"sin.v", fsin, data_type, ArithmeticOp.SIN>;
  defm ArithV : ArithUnaryVector<"cos.v", fcos, data_type, ArithmeticOp.COS>;
  defm ArithV : ArithUnaryVector<"exp.v", fexp, data_type, ArithmeticOp.EXP>;
  defm ArithV : ArithUnaryVector<"exp2.v", fexp2, data_type, ArithmeticOp.EXP2>;
  defm ArithV : ArithUnaryVector<"log.v", flog, data_type, ArithmeticOp.LOG>;
  defm ArithV : ArithUnaryVector<"log10.v", flog10, data_type, ArithmeticOp.LOG10>;
  defm ArithV : ArithUnaryVector<"log2.v", flog2, data_type, ArithmeticOp.LOG2>;
}

// ArithPred
defm ArithP : ArithPred<"and.p", and, ArithmeticOp.AND>;
defm ArithP : ArithPred<"or.p",  or, ArithmeticOp.OR>;
defm ArithP : ArithPred<"xor.p", xor, ArithmeticOp.XOR>;

// CmpScalar
foreach data_type = [DataType.Int32] in {
  defm CmpS : CmpScalar<"eq", [seteq], data_type, CompareOp.EQ>;
  defm CmpS : CmpScalar<"ne", [setne], data_type, CompareOp.NE>;
  defm CmpS : CmpScalar<"lt", [setlt, setult], data_type, CompareOp.LT>; // temporary fix, convert to signed compare
  defm CmpS : CmpScalar<"le", [setle, setule], data_type, CompareOp.LE>;
  defm CmpS : CmpScalar<"gt", [setgt, setugt], data_type, CompareOp.GT>;
  defm CmpS : CmpScalar<"ge", [setge, setuge], data_type, CompareOp.GE>;
}
foreach data_type = [DataType.Fp16, DataType.Fp32] in {
  defm CmpS : CmpScalar<"eq", [setoeq, setueq], data_type, CompareOp.EQ>;
  defm CmpS : CmpScalar<"ne", [setone, setune], data_type, CompareOp.NE>;
  defm CmpS : CmpScalar<"lt", [setolt, setult], data_type, CompareOp.LT>;
  defm CmpS : CmpScalar<"le", [setole, setule], data_type, CompareOp.LE>;
  defm CmpS : CmpScalar<"gt", [setogt, setugt], data_type, CompareOp.GT>;
  defm CmpS : CmpScalar<"ge", [setoge, setuge], data_type, CompareOp.GE>;
}

// CmpVector
foreach data_type = [DataType.Int32] in {
  defm CmpV : CmpVector<"eq.v", [seteq], data_type, CompareOp.EQ>;
  defm CmpV : CmpVector<"ne.v", [setne], data_type, CompareOp.NE>;
  defm CmpV : CmpVector<"lt.v", [setlt, setult], data_type, CompareOp.LT>; // temporary fix, convert to signed compare
  defm CmpV : CmpVector<"le.v", [setle, setule], data_type, CompareOp.LE>;
  defm CmpV : CmpVector<"gt.v", [setgt, setugt], data_type, CompareOp.GT>;
  defm CmpV : CmpVector<"ge.v", [setge, setuge], data_type, CompareOp.GE>;
}
foreach data_type = [DataType.Fp16, DataType.Fp32] in {
  defm CmpV : CmpVector<"eq.v", [setoeq, setueq], data_type, CompareOp.EQ>;
  defm CmpV : CmpVector<"ne.v", [setone, setune], data_type, CompareOp.NE>;
  defm CmpV : CmpVector<"lt.v", [setolt, setult], data_type, CompareOp.LT>;
  defm CmpV : CmpVector<"le.v", [setole, setule], data_type, CompareOp.LE>;
  defm CmpV : CmpVector<"gt.v", [setogt, setugt], data_type, CompareOp.GT>;
  defm CmpV : CmpVector<"ge.v", [setoge, setuge], data_type, CompareOp.GE>;
}

// CastScalar
defm CastS : CastScalar<"cast", sint_to_fp, CastType.Int32_To_Fp32>;
defm CastS : CastScalar<"cast", fp_to_sint, CastType.Fp32_To_Int32_Zero>;
defm CastS : CastScalar<"cast", fpround, CastType.Fp32_To_Fp16>;
defm CastS : CastScalar<"cast", fpextend, CastType.Fp16_To_Fp32>;
defm CastS : CastScalar<"cast", sint_to_fp, CastType.Int32_To_Fp16>;
defm CastS : CastScalar<"cast", fp_to_sint, CastType.Fp16_To_Int32_Zero>;

// 64bit
defm CastS : CastScalar<"cast", sext, CastType.Int32_To_Int64_Sign>;

// CastVector
defm CastV : CastVector<"cast.v", sint_to_fp, CastType.Int32_To_Fp32>;
defm CastV : CastVector<"cast.v", fp_to_sint, CastType.Fp32_To_Int32_Zero>;
defm CastV : CastVector<"cast.v", fpround, CastType.Fp32_To_Fp16>;
defm CastV : CastVector<"cast.v", fpextend, CastType.Fp16_To_Fp32>;
defm CastV : CastVector<"cast.v", sint_to_fp, CastType.Int32_To_Fp16>;
defm CastV : CastVector<"cast.v", fp_to_sint, CastType.Fp16_To_Int32_Zero>;

// ReduceArith
defm RedcueA : ReduceArith<"reduce", vp_reduce_add,  (i32 imm_zero),     DataType.Int32, ArithmeticOp.ADD>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fadd, (f32 fpimm_zero),   DataType.Fp32,  ArithmeticOp.ADD>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fadd, (f16 fpimm_zero),   DataType.Fp16,  ArithmeticOp.ADD>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_smax, (i32 imm_smin),     DataType.Int32, ArithmeticOp.MAX>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fmax, (f32 fpimm_lowest), DataType.Fp32,  ArithmeticOp.MAX>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fmax, (f16 fpimm_lowest), DataType.Fp16,  ArithmeticOp.MAX>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_smin, (i32 imm_smax),     DataType.Int32, ArithmeticOp.MIN>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fmin, (f32 fpimm_max),    DataType.Fp32,  ArithmeticOp.MIN>;
defm RedcueA : ReduceArith<"reduce", vp_reduce_fmin, (f16 fpimm_max),    DataType.Fp16,  ArithmeticOp.MIN>;


//ReduceShift
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_leftc_i, DataType.Int32, ShiftReduceType.Circular_Left>;
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_leftc_f, DataType.Fp32, ShiftReduceType.Circular_Left>;
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_leftc_h, DataType.Fp16, ShiftReduceType.Circular_Left>;
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_rightc_i, DataType.Int32, ShiftReduceType.Circular_Right>;
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_rightc_f, DataType.Fp32, ShiftReduceType.Circular_Right>;
defm ReduceSh : ReduceShift<"shift", int_drai_reduceshift_rightc_h, DataType.Fp16, ShiftReduceType.Circular_Right>;


// MoveScalar
defm MoveS : MoveScalar<"move", MoveType.SCALAR_REG_TO_SCALAR_REG>;
defm MoveS : MoveScalar<"move", MoveType.S64_REG_TO_S64_REG>;

// MoveVector
defm MoveV : MoveVector<"move", MoveType.VEC_REG_TO_VEC_REG>;
defm MoveV : MoveVector<"move", MoveType.PRED_REG_TO_PRED_REG>;
defm MoveV : MoveVector<"move", MoveType.SCALAR_REG_TO_VEC_REG>;
defm : MaskedMoveVector<MoveV_move_vv, 32>;
defm : MaskedMoveVector<MoveV_move_pp, 1>;

// MoveImmScalar
defm MoveI : MoveImmScalar<"move", MoveType.CONST_TO_SCALAR_REG>; // i32
def : Pat<(f32 fpimm:$val), (f32 (MoveI_move_si (fpimm_to_i32 f32:$val)))>; // f32
def : Pat<(f16 fpimm:$val), (f16 (MoveI_move_si (fpimm_to_i32 f16:$val)))>; // f16
def : Pat<(i1 -1), (i1 PredTruth)>; // Constant:i1<-1>
def : Pat<(i1 0), (ArithP_xor_p (i1 PredTruth), (i1 PredTruth), (i1 PredTruth), (i1 (IMPLICIT_DEF)))>; // Constant:i1<0>

// MoveImmExtra
defm MoveI : MoveImmExtra<"move", MoveType.CONST_TO_S64_REG>; // i64

// MoveImmSystem
defm MoveISys : MoveImmSystem<"move", MoveType.SPECIAL_TO_SCALAR_REG, SystemRegister.CoreIdx, int_drai_core_index>;
defm MoveISys : MoveImmSystem<"move", MoveType.SPECIAL_TO_SCALAR_REG, SystemRegister.CoreDim, int_drai_core_dim>;

// SelectValue
defm SelectS : SelectValueScalar<"select", mask_all, MoveType.SCALAR_REG_TO_SCALAR_REG, PredStateType.State_All>;
defm SelectS : SelectValueScalar<"select", mask_any, MoveType.SCALAR_REG_TO_SCALAR_REG, PredStateType.State_Any>;
defm SelectS : SelectValueScalar<"select", mask_none, MoveType.SCALAR_REG_TO_SCALAR_REG, PredStateType.State_None>;
defm SelectV : SelectValueVector<"select", MoveType.VEC_REG_TO_VEC_REG>;

// 64bit
defm SelectE : SelectValueExtra<"select", mask_all, MoveType.S64_REG_TO_S64_REG, PredStateType.State_All>;
defm SelectE : SelectValueExtra<"select", mask_any, MoveType.S64_REG_TO_S64_REG, PredStateType.State_Any>;
defm SelectE : SelectValueExtra<"select", mask_none, MoveType.S64_REG_TO_S64_REG, PredStateType.State_None>;

// Jmp
def JmpFirst : InstJmp<"jmp.first", PredStateType.State_First, [(brcond i1:$pred, bb:$value)]>;
def JmpAll : InstJmp<"jmp.all", PredStateType.State_All, [(ISDJmpAll vi1:$pred, bb:$value)]>;
def JmpAny : InstJmp<"jmp.any", PredStateType.State_Any, [(ISDJmpAny vi1:$pred, bb:$value)]>;
def JmpNone : InstJmp<"jmp.none", PredStateType.State_None, [(ISDJmpNone vi1:$pred, bb:$value)]>;
def : Pat<(br bb:$value), (JmpFirst (i1 PredTruth), bb:$value)>; // for uncondition jmp

// Loop
let isTerminator = 1, hasSideEffects = 1, mayLoad = 1, mayStore = 1 in {
def LoopPseudoSetIter : Pseudo<(outs), (ins i32imm:$id, i32imm:$depth, SRegOpnd:$lpc),
                               [(int_drai_loop_set_iter imm:$id, imm:$depth, i32:$lpc)]>;
def LoopPseudoIterDec : Pseudo<(outs PRegOpnd:$cond), (ins i32imm:$id),
                               [(set i1:$cond, (int_drai_loop_iter_dec imm:$id))]>;
}

def Loop : InstLoop;

// Return
def Return : InstReturn;

// SyncThreads
def SyncThreads : InstSyncThreads<SyncScope.None>;

// MemoryFence
def MemoryFence : InstMemoryFence<FenceScope.None>;

// Print
defm Print : PrintValue<"print", int_drai_print_s_i, RegisterType.Scalar, DataType.Int32>;
defm Print : PrintValue<"print", int_drai_print_s_f, RegisterType.Scalar, DataType.Fp32>;
defm Print : PrintValue<"print", int_drai_print_s_h, RegisterType.Scalar, DataType.Fp16>;
defm Print : PrintValue<"print", int_drai_print_v_i, RegisterType.Vector, DataType.Int32>;
defm Print : PrintValue<"print", int_drai_print_v_f, RegisterType.Vector, DataType.Fp32>;
defm Print : PrintValue<"print", int_drai_print_v_h, RegisterType.Vector, DataType.Fp16>;
defm Print : PrintValue<"print", int_drai_print_v_b, RegisterType.Pred, DataType.None>;
defm Print : PrintValue<"print", int_drai_print_e, RegisterType.EScalar, DataType.Int64>;

// Equivalent substitution

// Fneg (using arith sub)
def : Pat<(vf32:$rout (vselect vi1:$pred, (fneg vf32:$in1), vf32:$in2)),  // fp32
          (vf32:$rout (ArithV_sub_v_f32 (vf32:$rout (MoveV_move_vs (f32:$rout (CastS_cast_f32_i32
          (i32:$rout (MoveI_move_si (i32 0x0000))))), (vi1 PredTruth), vf32:$in2)), vf32:$in1, vi1:$pred, vf32:$in2))>;
def : Pat<(vf16:$rout (vselect vi1:$pred, (fneg vf16:$in1), vf16:$in2)),  // fp16
          (vf16:$rout (ArithV_sub_v_f16 (vf16:$rout (MoveV_move_vs (f16:$rout (CastS_cast_f16_i32
          (i32:$rout (MoveI_move_si (i32 0x0000))))), (vi1 PredTruth), vf16:$in2)), vf16:$in1, vi1:$pred, vf16:$in2))>;

/*
we do't support srl
InstCombineSimplifyDemanded.cpp, InstCombinerImpl::SimplifyDemandedUseBits, case Instruction::AShr
replacing:
  res = srl input, len
with:
  num = sub 32, len
  mask1 = shl 1, num
  mask = sub mask1, 1
  tmp = sra input, len
  res = and tmp, mask
*/
def : Pat<(i32:$out (srl i32:$input, i32:$len)),
          (i32:$out (ArithS_and_i32
            (ArithS_shr_i32 i32:$input, i32:$len),
            (ArithS_sub_i32
              (ArithS_shl_i32
                (MoveI_move_si (i32 1)),
                (ArithS_sub_i32 (MoveI_move_si (i32 32)), i32:$len)
              ),
              (MoveI_move_si (i32 1))
            )
          ))>;


def : Pat<(vi32:$out (srl vi32:$input, vi32:$len)),
          (vi32:$out (ArithV_and_v_i32
            (ArithV_shr_v_i32 vi32:$input, vi32:$len, (vi1 PredTruth), (vi32 (IMPLICIT_DEF))),
            (ArithV_sub_v_i32
              (ArithV_shl_v_i32
                (MoveV_move_vs (MoveI_move_si (i32 1)), (vi1 PredTruth), (vi32 (IMPLICIT_DEF))),
                (ArithV_sub_v_i32
                  (MoveV_move_vs (MoveI_move_si (i32 32)), (vi1 PredTruth), (vi32 (IMPLICIT_DEF))),
                  vi32:$len,
                  (vi1 PredTruth),
                  (vi32 (IMPLICIT_DEF))
                ),
                (vi1 PredTruth),
                (vi32 (IMPLICIT_DEF))
              ),
              (MoveV_move_vs (MoveI_move_si (i32 1)), (vi1 PredTruth), (vi32 (IMPLICIT_DEF))),
              (vi1 PredTruth),
              (vi32 (IMPLICIT_DEF))
            ),
            (vi1 PredTruth),
            (vi32 (IMPLICIT_DEF))
          ))>;

// select: i1 != i1
def : Pat<(i1:$out (setne i1:$in1, i1:$in2)),
          (i1:$out (ArithP_xor_p i1:$in1, i1:$in2, (i1 PredTruth), (i1 (IMPLICIT_DEF))))>;

// select: i1 == i1
def : Pat<(i1:$out (seteq i1:$in1, i1:$in2)),
          (i1:$out (ArithP_xor_p
                      (ArithP_xor_p i1:$in1, i1:$in2, (i1 PredTruth), (i1 (IMPLICIT_DEF))),
                      (i1 PredTruth),
                      (i1 PredTruth),
                      (i1 (IMPLICIT_DEF))
                    ))>;
