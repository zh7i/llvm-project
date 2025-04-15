class InstDRAI<dag outs, dag ins, string asmstr, list<dag> pattern,
             InstrItinClass itin = NoItinerary>
   : Instruction {
  bits<64> Inst;

  // field bits<64> SoftFail = 0; // Needed by disassembler.

  let Namespace = "DRAI";
  let Size = 8;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;

  let DecoderNamespace = "DRAI";

  let Itinerary = itin;

  let hasSideEffects = 0;
  let mayLoad        = 0;
  let mayStore       = 0;
}

class Pseudo<dag outs, dag ins, list<dag> pattern>
  : InstDRAI<outs, ins, "pseudo!!", pattern> {
  let isCodeGenOnly       = 1;
  let isPseudo            = 1;
  let hasNoSchedulingInfo = 1;
}

// Instruction format

class InstSIMD<dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin = NoItinerary> : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  dtype;
  bits<16> attr;
  bits<6>  rin1;
  bits<6>  rin2;
  bits<6>  rout;
  bits<12> reserved = 0x0;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = dtype;
  let Inst{33-18} = attr;
  let Inst{39-34} = rin1;
  let Inst{45-40} = rin2;
  let Inst{51-46} = rout;
  let Inst{63-52} = reserved;
}

class InstStorage<dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin = NoItinerary> : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  op_type;
  bits<16> attr;
  bits<6>  rin1;
  bits<6>  rin2;
  bits<6>  rout;
  bits<12> reserved = 0x0;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = op_type;
  let Inst{33-18} = attr;
  let Inst{39-34} = rin1;
  let Inst{45-40} = rin2;
  let Inst{51-46} = rout;
  let Inst{63-52} = reserved;
}

class InstMoveImm<dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin = NoItinerary> : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  op_type;
  bits<6>  rout;
  bits<8>  reserved = 0x0;
  bits<32> value;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = op_type;
  let Inst{23-18} = rout;
  let Inst{31-24} = reserved;
  let Inst{63-32} = value;
}

class InstMoveImmExtraFormat<dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin = NoItinerary> : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  op_type;
  bits<6>  rout;
  bits<64> value;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = op_type;
  let Inst{23-18} = rout;
  let Inst{63-24} = value{39-0};
}

class InstSpecial<dag outs, dag ins, string asmstr,
                  list<dag> pattern, InstrItinClass itin = NoItinerary>
  : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  dtype;
  bits<8>  attr;
  bits<6>  reserved = 0x0;
  bits<32> value;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = dtype;
  let Inst{25-18} = attr;
  let Inst{31-26} = reserved;
  let Inst{63-32} = value;
}

class InstLoopFormat<dag outs, dag ins, string asmstr,
                  list<dag> pattern, InstrItinClass itin = NoItinerary>
  : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  depth;
  bits<4>  reserved  = 0x0;
  bits<6>  lpc;
  bits<8>  reserved2 = 0x0;
  bits<32> lpend;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = depth;
  let Inst{17-14} = reserved;
  let Inst{23-18} = lpc;
  let Inst{31-24} = reserved2;
  let Inst{63-32} = lpend;
}

class InstCModel<dag outs, dag ins, string asmstr, // for cmodel debug
                 list<dag> pattern, InstrItinClass itin = NoItinerary>
  : InstDRAI<outs, ins, asmstr, pattern, itin> {
  bits<8>  op_code;
  bits<6>  pred;
  bits<4>  dtype;
  bits<8>  attr;
  bits<6>  reg;
  bits<32> value;

  let Inst{7-0}   = op_code;
  let Inst{13-8}  = pred;
  let Inst{17-14} = dtype;
  let Inst{25-18} = attr;
  let Inst{31-26} = reg;
  let Inst{63-32} = value;
}

// Some node and pattern
def brtarget : Operand<OtherVT>;

def ISDRetFlag : SDNode<"DRAIISD::RET_FLAG", SDTNone,
                        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>; // to match special dag node
def ISDJmpAll : SDNode<"DRAIISD::JmpAll",
                       SDTypeProfile<0 /* res num */, 2 /* opnd num */,
                       [SDTCisVT<0, vi1>, SDTCisVT<1, OtherVT> /* opnd type */ /* res type */]>,
                       [SDNPHasChain] /* when void ret */>;
def ISDJmpAny : SDNode<"DRAIISD::JmpAny",
                       SDTypeProfile<0, 2,
                       [SDTCisVT<0, vi1>, SDTCisVT<1, OtherVT>]>,
                       [SDNPHasChain]>;
def ISDJmpNone : SDNode<"DRAIISD::JmpNone",
                        SDTypeProfile<0, 2,
                        [SDTCisVT<0, vi1>, SDTCisVT<1, OtherVT>]>,
                        [SDNPHasChain]>;

def fpimm_to_i32 : SDNodeXForm<fpimm, [{
  return CurDAG->getTargetConstant(
    N->getValueAPF().bitcastToAPInt().getZExtValue(), SDLoc(N), MVT::i32);
}]>;

def i64imm_to_i32 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant(
    N->getZExtValue(), SDLoc(N), MVT::i64);
}]>;

def fexp      : SDNode<"ISD::FEXP"      , SDTFPUnaryOp>;
def flog      : SDNode<"ISD::FLOG"      , SDTFPUnaryOp>;
def flog10    : SDNode<"ISD::FLOG10"    , SDTFPUnaryOp>;
def vecreduce_fmax  : SDNode<"ISD::VECREDUCE_FMAX", SDTFPVecReduce>;
def vecreduce_fmin  : SDNode<"ISD::VECREDUCE_FMIN", SDTFPVecReduce>;

def imm_zero : ImmLeaf<iAny, [{
  return Imm == 0;
}]>;
def imm_smax : ImmLeaf<iAny, [{
  return Imm == 2147483647; // 0x7fffffff
}]>;
def imm_smin : ImmLeaf<iAny, [{
  return Imm == -2147483648; // 0x80000000
}]>;

def fpimm_zero : FPImmLeaf<fAny, [{
  return Imm.isZero();
}]>;
def fpimm_max : FPImmLeaf<fAny, [{
  return Imm.isFinite() && Imm.isLargest();
}]>;
def fpimm_lowest : FPImmLeaf<fAny, [{
  return Imm.isFinite() && Imm.isLargest() && Imm.isNegative();
}]>;

def SDTVPReduce : SDTypeProfile<1, 4, [
  SDTCisInt<0>,
  SDTCisInt<1>,
  SDTCisVec<2>,
  SDTCisVec<3>,
  SDTCisInt<4>,
  SDTCisSameAs<0, 1>,
  SDTCisSameNumEltsAs<2, 3>,
]>;

def SDTFPVPReduce : SDTypeProfile<1, 4, [
  SDTCisFP<0>,
  SDTCisFP<1>,
  SDTCisVec<2>,
  SDTCisVec<3>,
  SDTCisInt<4>,
  SDTCisSameAs<0, 1>,
  SDTCisSameNumEltsAs<2, 3>,
]>;

def vp_reduce_add : SDNode<"ISD::VP_REDUCE_ADD", SDTVPReduce>;
def vp_reduce_fadd : SDNode<"ISD::VP_REDUCE_FADD", SDTFPVPReduce>;
def vp_reduce_smax : SDNode<"ISD::VP_REDUCE_SMAX", SDTVPReduce>;
def vp_reduce_fmax : SDNode<"ISD::VP_REDUCE_FMAX", SDTFPVPReduce>;
def vp_reduce_smin : SDNode<"ISD::VP_REDUCE_SMIN", SDTVPReduce>;
def vp_reduce_fmin : SDNode<"ISD::VP_REDUCE_FMIN", SDTFPVPReduce>;

def mask_all : PatFrag<(ops node:$pred), (i1 (seteq (im_ty (bitconvert node:$pred)), -1))>;
def mask_any : PatFrag<(ops node:$pred), (i1 (setne (im_ty (bitconvert node:$pred)), 0))>;
def mask_none : PatFrag<(ops node:$pred), (i1 (seteq (im_ty (bitconvert node:$pred)), 0))>;

class load_as<AddressSpaceEnumValue as> :
        PatFrag<(ops node:$ptr), (unindexedload node:$ptr)> {
  let IsLoad = 1;
  let IsNonExtLoad = 1;
  let AddressSpaces = [ as.val ];
}

class store_as<AddressSpaceEnumValue as> :
        PatFrag<(ops node:$val, node:$ptr), (unindexedstore node:$val, node:$ptr)> {
  let IsStore = 1;
  let IsTruncStore = 0;
  let AddressSpaces = [ as.val ];
}

class masked_load_as<AddressSpaceEnumValue as> :
        PatFrag<(ops node:$ptr, node:$pred, node:$def),
                (masked_ld node:$ptr, undef, node:$pred, node:$def), [{
    const auto *Node = cast<MaskedLoadSDNode>(N);
    return Node->isUnindexed() && Node->getExtensionType() == ISD::NON_EXTLOAD;
  }]> {
  let IsLoad = 1;
  // let IsNonExtLoad = 1; // only LoadSDNode support: llvm/utils/TableGen/CodeGenDAGPatterns.cpp
  // let IsUnindexed = 1;
  let AddressSpaces = [ as.val ];
}

class masked_store_as<AddressSpaceEnumValue as> :
      PatFrag<(ops node:$val, node:$ptr, node:$pred),
              (masked_st node:$val, node:$ptr, undef, node:$pred), [{
    const auto *Node = cast<MaskedStoreSDNode>(N);
    return Node->isUnindexed() && !Node->isTruncatingStore();
  }]> {
  let IsStore = 1;
  // let IsTruncStore = 0; // only StoreSDNode support
  // let IsUnindexed = 1;
  let AddressSpaces = [ as.val ];
}

class masked_gather_as<AddressSpaceEnumValue as> :
      PatFrag<(ops node:$def, node:$pred, node:$ptr, node:$idx),
              (masked_gather node:$def, node:$pred, node:$ptr, node:$idx), [{
    const auto *Node = cast<MaskedGatherSDNode>(N);
    return Node->getExtensionType() == ISD::NON_EXTLOAD;
  }]> {
  let IsLoad = 1;
  // let IsNonExtLoad = 1; // only LoadSDNode support
  let AddressSpaces = [ as.val ];
}

class masked_scatter_as<AddressSpaceEnumValue as> :
      PatFrag<(ops node:$val, node:$pred, node:$ptr, node:$idx),
              (masked_scatter node:$val, node:$pred, node:$ptr, node:$idx), [{
    const auto *Node = cast<MaskedScatterSDNode>(N);
    return !Node->isTruncatingStore();
  }]> {
  let IsStore = 1;
  // let IsTruncStore = 0; // only StoreSDNode support
  let AddressSpaces = [ as.val ];
}

class as_match<AddressSpaceEnumValue as> {
  code global_pred = [{
    const auto *Node = cast<MemIntrinsicSDNode>(N);
    return Node->getAddressSpace() == (unsigned)DRAIAddressSpace::ADDRSPACE_GLOBAL;
  }];
  code shared_pred = [{
    const auto *Node = cast<MemIntrinsicSDNode>(N);
    return Node->getAddressSpace() == (unsigned)DRAIAddressSpace::ADDRSPACE_SHARED;
  }];

  code pred = !cond(
    !eq(as, AddressSpace.Global) : global_pred,
    !eq(as, AddressSpace.Shared) : shared_pred
  );
}

class scatter_as<Intrinsic intr, AddressSpaceEnumValue as> :
      PatFrag<(ops node:$ptr, node:$idx, node:$data, node:$mask, node:$op),
              (intr node:$ptr, node:$idx, node:$data, node:$mask, node:$op),
              as_match<as>.pred>;

// Same opcode instructions
class InstSIMDIndex :
          InstSIMD<(outs VRegOpnd:$rout), (ins),
                    !strconcat("index", " $rout"),
                    [(set vi32:$rout, (int_drai_thread_index))]> {
  let op_code = CmdType.Index.val;
  let pred    = 0x0;
  let dtype   = DataType.Int32.val;
  let attr    = 0x0;
  let rin1    = 0x0;
  let rin2    = 0x0;
}

class InstSIMDGather<string inst_asm, DataTypeEnumValue data_type, AddressSpaceEnumValue as, list<dag> pattern> :
          InstSIMD<(outs VRegOpnd:$rout), (ins VRegOpnd:$rin1, ERegOpnd:$rin2, PRegOpndPrt:$pred, VRegOpnd:$old),
                    !strconcat(inst_asm, " $rout, $rin2[$rin1]   $pred"),
                    pattern> {
  let mayLoad = 1;

  let op_code = CmdType.Gather_Fast.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !shl(!cast<bits<4>>(as.val), 12);
}
multiclass SIMDGather<string asm,
                      DataTypeEnumValue data_type,
                      AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, as.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));
  defvar pat = [(set vt:$rout, (masked_gather_as<as> vt:$old, vi1:$pred, i64:$rin2, vi32:$rin1))];

  def NAME # suffix : InstSIMDGather<inst_asm, data_type, as, pat>;
}

class InstSIMDScatter<string inst_asm, DataTypeEnumValue data_type, ArithmeticOpEnumValue op_type, AddressSpaceEnumValue as> :
          InstSIMD<(outs), (ins VRegOpnd:$rin1, VRegOpnd:$rin2, ERegOpnd:$rout, PRegOpndPrt:$pred),
                    !strconcat(inst_asm, " $rout[$rin1], $rin2   $pred"),
                    []> {
  let mayStore = 1;

  let op_code = CmdType.Scatter_Fast.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(op_type.val), !shl(!cast<bits<4>>(as.val), 12));
}
multiclass SIMDScatter<string asm,
                       DataTypeEnumValue data_type,
                       AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, as.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDScatter<inst_asm, data_type, ArithmeticOp.NONE, as>;

  // manual match
  def : Pat<(masked_scatter_as<as> vt:$rin2, vi1:$pred, i64:$rout, vi32:$rin1),
            (!cast<Instruction>(NAME # suffix) vi32:$rin1, vt:$rin2, i64:$rout, vi1:$pred)>;
}
multiclass SIMDScatterOper<string asm,
                           Intrinsic intr,
                           DataTypeEnumValue data_type,
                           ArithmeticOpEnumValue op_type,
                           AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, as.asm, data_type.asm, op_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  let mayLoad = 1 in
  def NAME # suffix : InstSIMDScatter<inst_asm, data_type, op_type, as>;

  // manual match
  def : Pat<(scatter_as<intr, as> i64:$rout, vi32:$rin1, vt:$rin2, vi1:$pred, (i32 op_type.val)),
            (!cast<Instruction>(NAME # suffix) vi32:$rin1, vt:$rin2, i64:$rout, vi1:$pred)>;
}

class InstSIMDLoadScalar<string inst_asm, RegisterOperand opnd, DataTypeEnumValue data_type,
                         MoveTypeEnumValue move_type, AddressSpaceEnumValue as, list<dag> pattern> :
          InstSIMD<(outs opnd:$rin2), (ins ERegOpnd:$rin1),
                    !strconcat(inst_asm, " $rin2, [$rin1]"), // improper
                    pattern> {
  let mayLoad = 1;

  let op_code = CmdType.Load_Fast.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(move_type.val), !shl(!cast<bits<4>>(as.val), 12));
  let rout    = 0x0;
}
multiclass LoadScalar<string asm,
                      DataTypeEnumValue data_type,
                      MoveTypeEnumValue move_type,
                      AddressSpaceEnumValue as> {
  defvar opnd = data_type.opnd_s;
  defvar vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, as.asm, move_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));
  defvar pat = [(set vt:$rin2, (load_as<as> i64:$rin1))];

  def NAME # suffix : InstSIMDLoadScalar<inst_asm, opnd, data_type, move_type, as, pat>;
}

class InstSIMDLoadVector<string inst_asm, DataTypeEnumValue data_type,
                         MoveTypeEnumValue move_type, AddressSpaceEnumValue as, list<dag> pattern> :
          InstSIMD<(outs VRegOpnd:$rin2), (ins ERegOpnd:$rin1, PRegOpndPrt:$pred, VRegOpnd:$old),
                    !strconcat(inst_asm, " $rin2, [$rin1]   $pred"), // improper
                    pattern> {
  let mayLoad = 1;
  let Constraints = "$rin2 = $old"; // only support inplace overwrite

  let op_code = CmdType.Load_Fast.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(move_type.val), !shl(!cast<bits<4>>(as.val), 12));
  let rout    = 0x0;
}
multiclass LoadVector<string asm,
                      DataTypeEnumValue data_type,
                      MoveTypeEnumValue move_type,
                      AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, as.asm, move_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));
  defvar pat = [(set vt:$rin2, (masked_load_as<as> i64:$rin1, vi1:$pred, vt:$old))];

  def NAME # suffix : InstSIMDLoadVector<inst_asm, data_type, move_type, as, pat>;

  // match not predicted
  def : Pat<(vt:$rin2 (load_as<as> i64:$rin1)),
            (vt:$rin2 (!cast<Instruction>(NAME # suffix) i64:$rin1, (vi1 PredTruth), (vt (IMPLICIT_DEF))))>;
  // match meta operator
  def : Pat<(vt:$rin2 (vselect vi1:$pred, (vt (load_as<as> i64:$rin1)), vt:$old)),
            (vt:$rin2 (!cast<Instruction>(NAME # suffix) i64:$rin1, vi1:$pred, vt:$old))>;
}

class InstSIMDStoreScalar<string inst_asm, DataTypeEnumValue data_type,
                          MoveTypeEnumValue move_type, AddressSpaceEnumValue as, list<dag> pattern> :
          InstSIMD<(outs), (ins SRegOpnd:$rin1, ERegOpnd:$rin2, PRegOpndPrt:$pred),
                    !strconcat(inst_asm, " [$rin2], $rin1   $pred"), // improper
                    pattern> { // todo simplify this
  let mayLoad  = 1; // fused load
  let mayStore = 1;

  let op_code = CmdType.Store_Fast.val;
  // let pred    = PredTruth.enc;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(move_type.val), !shl(!cast<bits<4>>(as.val), 12));
  let rout    = 0x0;
}
multiclass StoreScalar<string asm,
                       DataTypeEnumValue data_type,
                       MoveTypeEnumValue move_type,
                       AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, as.asm, move_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));
  defvar pat = [(store_as<as> (select (mask_any vi1:$pred), vt:$rin1, (vt (load_as<as> i64:$rin2))), i64:$rin2)];

  def NAME # suffix : InstSIMDStoreScalar<inst_asm, data_type, move_type, as, pat>;

  // match not predicted
  def : Pat<(store_as<as> vt:$rin1, i64:$rin2),
            (!cast<Instruction>(NAME # suffix) vt:$rin1, i64:$rin2, (vi1 PredTruth))>;
  // match mask_none
  def : Pat<(store_as<as> (select (mask_none vi1:$pred), (vt (load_as<as> i64:$rin2)), vt:$rin1), i64:$rin2),
            (!cast<Instruction>(NAME # suffix) vt:$rin1, i64:$rin2, vi1:$pred)>;
}

class InstSIMDStoreVector<string inst_asm, DataTypeEnumValue data_type,
                          MoveTypeEnumValue move_type, AddressSpaceEnumValue as, list<dag> pattern> :
          InstSIMD<(outs), (ins VRegOpnd:$rin1, ERegOpnd:$rin2, PRegOpndPrt:$pred),
                    !strconcat(inst_asm, " [$rin2], $rin1   $pred"), // improper
                    pattern> { // todo simplify this
  let mayLoad  = 1; // fused load
  let mayStore = 1;

  let op_code = CmdType.Store_Fast.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(move_type.val), !shl(!cast<bits<4>>(as.val), 12));
  let rout    = 0x0;
}
multiclass StoreVector<string asm,
                       DataTypeEnumValue data_type,
                       MoveTypeEnumValue move_type,
                       AddressSpaceEnumValue as> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, as.asm, move_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));
  defvar pat = [(masked_store_as<as> vt:$rin1, i64:$rin2, vi1:$pred)];

  def NAME # suffix : InstSIMDStoreVector<inst_asm, data_type, move_type, as, pat>;

  // match not predicted
  def : Pat<(store_as<as> vt:$rin1, i64:$rin2),
            (!cast<Instruction>(NAME # suffix) vt:$rin1, i64:$rin2, (vi1 PredTruth))>;
  // match mask_none
  def : Pat<(store_as<as> (vselect vi1:$pred, vt:$rin1, (vt (load_as<as> i64:$rin2))), i64:$rin2),
            (!cast<Instruction>(NAME # suffix) vt:$rin1, i64:$rin2, vi1:$pred)>;
}

class InstSIMDArithScalar<string inst_asm, SDNode op_node, RegisterOperand opnd, ValueType vt, DataTypeEnumValue data_type, ArithmeticOpEnumValue arith_op> :
          InstSIMD<(outs opnd:$rout), (ins opnd:$rin1, opnd:$rin2),
                   !strconcat(inst_asm, " $rout, $rin1, $rin2"),
                   [(set vt:$rout, (op_node vt:$rin1, vt:$rin2))]>{
  let op_code = CmdType.Arithmetic_S.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = arith_op.val;
}
multiclass ArithScalar<string asm,
                       SDNode op_node,
                       DataTypeEnumValue data_type,
                       ArithmeticOpEnumValue arith_op> {
  defvar opnd = data_type.opnd_s;
  defvar vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDArithScalar<inst_asm, op_node, opnd, vt, data_type, arith_op>;
}

class InstSIMDArithVector<string inst_asm, SDNode op_node, ValueType vt, DataTypeEnumValue data_type, ArithmeticOpEnumValue arith_op> :
          InstSIMD<(outs VRegOpnd:$rout), (ins VRegOpnd:$rin1, VRegOpnd:$rin2, PRegOpndPrt:$pred, VRegOpnd:$old),
                   !strconcat(inst_asm, " $rout, $rin1, $rin2   $pred"),
                   [(set vt:$rout, (vselect vi1:$pred, (op_node vt:$rin1, vt:$rin2), vt:$old))]>{
  let Constraints = "$rout = $old"; // only support inplace overwrite

  let op_code = CmdType.Arithmetic_V.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(arith_op.val), !shl(!cast<bits<8>>(RegRegType.VV.val), 8)); // only support vec-vec
}
multiclass ArithVector<string asm,
                       SDNode op_node,
                       DataTypeEnumValue data_type,
                       ArithmeticOpEnumValue arith_op> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDArithVector<inst_asm, op_node, vt, data_type, arith_op>;

  // match not predicted
  def : Pat<(vt:$rout (op_node vt:$rin1, vt:$rin2)),
            (vt:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2, (vi1 PredTruth), (vt (IMPLICIT_DEF))))>;
}

class InstSIMDArithUnaryScalar<string inst_asm, SDNode op_node, RegisterOperand opnd, ValueType vt, DataTypeEnumValue data_type, ArithmeticOpEnumValue arith_op> :
          InstSIMD<(outs opnd:$rout), (ins opnd:$rin1),
                   !strconcat(inst_asm, " $rout, $rin1"),
                   [(set vt:$rout, (op_node vt:$rin1))]>{
  let op_code = CmdType.Arithmetic_S.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = arith_op.val;
  let rin2    = 0x0;
}
multiclass ArithUnaryScalar<string asm,
                            SDNode op_node,
                            DataTypeEnumValue data_type,
                            ArithmeticOpEnumValue arith_op> {
  defvar opnd = data_type.opnd_s;
  defvar vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDArithUnaryScalar<inst_asm, op_node, opnd, vt, data_type, arith_op>;
}

class InstSIMDArithUnaryVector<string inst_asm, SDNode op_node, ValueType vt, DataTypeEnumValue data_type, ArithmeticOpEnumValue arith_op> :
          InstSIMD<(outs VRegOpnd:$rout), (ins VRegOpnd:$rin1),
                   !strconcat(inst_asm, " $rout, $rin1"),
                   [(set vt:$rout, (op_node vt:$rin1))]>{
  let op_code = CmdType.Arithmetic_V.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(arith_op.val), !shl(!cast<bits<8>>(RegRegType.VV.val), 8)); // only support vec-vec
  let rin2    = 0x0;
}
multiclass ArithUnaryVector<string asm,
                            SDNode op_node,
                            DataTypeEnumValue data_type,
                            ArithmeticOpEnumValue arith_op> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDArithUnaryVector<inst_asm, op_node, vt, data_type, arith_op>;
}

class InstSIMDArithPred<string inst_asm, ArithmeticOpEnumValue arith_op> :
          InstSIMD<(outs PRegOpnd:$rout), (ins PRegOpnd:$rin1, PRegOpnd:$rin2, PRegOpndPrt:$pred, PRegOpnd:$old),
                   !strconcat(inst_asm, " $rout, $rin1, $rin2   $pred"),
                   []>{
  let op_code = CmdType.Arithmetic_P.val;
  // let pred    = 0x0;
  let dtype   = 0x0;
  let attr    = arith_op.val;
}
multiclass ArithPred<string asm,
                     SDNode op_node,
                     ArithmeticOpEnumValue arith_op> {
  defvar inst_asm = asm;
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDArithPred<inst_asm, arith_op>;

  foreach vt = [i1, vi1] in {
    defvar sel_node = !cond(!eq(vt, i1) : select, !eq(vt, vi1) : vselect);

    def : Pat<(vt:$rout (sel_node vt:$pred, (op_node vt:$rin1, vt:$rin2), vt:$old)),
              (vt:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2, vt:$pred, vt:$old))>;

    // match not predicted
    def : Pat<(vt:$rout (op_node vt:$rin1, vt:$rin2)),
              (vt:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2, (vt PredTruth), (vt (IMPLICIT_DEF))))>;
  }
}

class InstSIMDCmpScalar<string inst_asm, DataTypeEnumValue data_type, CompareOpEnumValue cmp_op> :
          InstSIMD<(outs PRegOpnd:$rout), (ins SRegOpnd:$rin1, SRegOpnd:$rin2),
                   !strconcat(inst_asm, " $rout, $rin1, $rin2"),
                   []> {
  let op_code = CmdType.Compare_S.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = cmp_op.val;
}

multiclass CmpScalar<string asm,
                     list<PatFrag> cond_ops,
                     DataTypeEnumValue data_type,
                     CompareOpEnumValue cmp_op> {
  defvar vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDCmpScalar<inst_asm, data_type, cmp_op>;

  foreach cond_op = cond_ops in
  def : Pat<(i1:$rout (cond_op vt:$rin1, vt:$rin2)), (i1:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2))>;
}

class InstSIMDCmpVector<string inst_asm, DataTypeEnumValue data_type, CompareOpEnumValue cmp_op> :
          InstSIMD<(outs PRegOpnd:$rout), (ins VRegOpnd:$rin1, VRegOpnd:$rin2, PRegOpndPrt:$pred, PRegOpnd:$old),
                   !strconcat(inst_asm, " $rout, $rin1, $rin2   $pred"),
                   []>{
  let op_code = CmdType.Compare_V.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<8>>(cmp_op.val), !shl(!cast<bits<8>>(RegRegType.VV.val), 8)); // only support vec-vec
}
multiclass CmpVector<string asm,
                     list<PatFrag> cond_ops,
                     DataTypeEnumValue data_type,
                     CompareOpEnumValue cmp_op> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDCmpVector<inst_asm, data_type, cmp_op>;

  foreach cond_op = cond_ops in {
    def : Pat<(vi1:$rout (vselect vi1:$pred, (cond_op vt:$rin1, vt:$rin2), vi1:$old)),
              (vi1:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2, vi1:$pred, vi1:$old))>;

    // match not predicted
    def : Pat<(vi1:$rout (cond_op vt:$rin1, vt:$rin2)),
              (vi1:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, vt:$rin2, (vi1 PredTruth), (vi1 (IMPLICIT_DEF))))>;
  }
}

class InstSIMDCastScalar<string inst_asm, SDNode op_node, RegisterOperand dst_opnd, RegisterOperand src_opnd, ValueType dst_vt, ValueType src_vt, CastTypeEnumValue cast_type> :
          InstSIMD<(outs dst_opnd:$rout), (ins src_opnd:$rin1),
                   !strconcat(inst_asm, " $rout, $rin1"),
                   [(set dst_vt:$rout, (op_node src_vt:$rin1))]>{
  let op_code = CmdType.Cast_S.val;
  let pred    = 0x0;
  let dtype   = 0x0;
  let attr    = cast_type.val;
  let rin2    = 0x0;
}
multiclass CastScalar<string asm,
                      SDNode op_node,
                      CastTypeEnumValue cast_type> {
  defvar src_vt = cast_type.src_vt_s;
  defvar dst_vt = cast_type.dst_vt_s;
  defvar src_opnd = cast_type.src_opnd_s;
  defvar dst_opnd = cast_type.dst_opnd_s;
  defvar inst_asm = !strconcat(asm, cast_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDCastScalar<inst_asm, op_node, dst_opnd, src_opnd, dst_vt, src_vt, cast_type>;
}

class InstSIMDCastVector<string inst_asm, SDNode op_node, ValueType dst_vt, ValueType src_vt, CastTypeEnumValue cast_type> :
          InstSIMD<(outs VRegOpnd:$rout), (ins VRegOpnd:$rin1),
                   !strconcat(inst_asm, " $rout, $rin1"),
                   [(set dst_vt:$rout, (op_node src_vt:$rin1))]>{
  let op_code = CmdType.Cast_V.val;
  let pred    = 0x0;
  let dtype   = 0x0;
  let attr    = cast_type.val;
  let rin2    = 0x0;
}
multiclass CastVector<string asm,
                      SDNode op_node,
                      CastTypeEnumValue cast_type> {
  defvar src_vt = cast_type.src_vt_v;
  defvar dst_vt = cast_type.dst_vt_v;
  defvar inst_asm = !strconcat(asm, cast_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDCastVector<inst_asm, op_node, dst_vt, src_vt, cast_type>;
}

class InstSIMDReduceArith<string inst_asm, DataTypeEnumValue data_type, ArithmeticOpEnumValue arith_type> :
          InstSIMD<(outs SRegOpnd:$rout), (ins VRegOpnd:$rin1, PRegOpnd:$pred),
                    !strconcat(inst_asm, " $rout, $rin1   $pred"),
                    []>{
  let op_code = CmdType.Arith_Reduce.val;
  // let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = arith_type.val;
  let rin2    = 0x0;
}
multiclass ReduceArith<string asm,
                      SDNode vp_node,
                      dag ident,
                      DataTypeEnumValue data_type,
                      ArithmeticOpEnumValue arith_type> {
  defvar src_vt = data_type.vt_v;
  defvar dst_vt = data_type.vt_s;
  defvar inst_asm = !strconcat(asm, arith_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDReduceArith<inst_asm, data_type, arith_type>;

  def : Pat<(dst_vt:$rout (vp_node ident, src_vt:$rin1, vi1:$pred, (i32 simd_width))),
            (dst_vt:$rout (!cast<Instruction>(NAME # suffix) src_vt:$rin1, vi1:$pred))>;

  // // match simplified vector reduce intrinsic
  // def : Pat<(dst_vt:$rout (vec_node src_vt:$rin1)),
  //           (dst_vt:$rout (!cast<Instruction>(NAME # suffix) src_vt:$rin1, (vi1 PredTruth)))>;
}

// Shift reduce
class InstSIMDReduceShift<string inst_asm, DataTypeEnumValue data_type, ShiftReduceTypeEnumValue shift_type> :
          InstSIMD<(outs VRegOpnd:$rout), (ins VRegOpnd:$rin1, SRegOpnd:$rin2),
                    !strconcat(inst_asm, " $rout, $rin1, $rin2"),
                    []>{
  let op_code = CmdType.Shift_Reduce.val;
  let pred    = 0x0;						        // pred not required?
  let dtype   = data_type.val;					// int32, fp32, fp16
  let attr    = shift_type.val;					// left circular, right circular
}
multiclass ReduceShift<string asm,
                      Intrinsic intr,
                      DataTypeEnumValue data_type,
                      ShiftReduceTypeEnumValue shift_type> {
  defvar vt = data_type.vt_v;
  defvar inst_asm = !strconcat(asm, shift_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSIMDReduceShift<inst_asm, data_type, shift_type>;

  def : Pat<(vt:$rout (intr vt:$rin1, i32:$rin2)),
            (vt:$rout (!cast<Instruction>(NAME # suffix) vt:$rin1, i32:$rin2))>;
}

class InstMoveScalar<string inst_asm, RegisterOperand dst_opnd, RegisterOperand src_opnd, MoveTypeEnumValue move_type> :
          InstStorage<(outs dst_opnd:$rin2), (ins src_opnd:$rin1),
                      !strconcat(inst_asm, " $rin2, $rin1"), // improper
                      []> { // maybe it's a bug
  let isMoveReg = 1;

  let op_code = CmdType.Move.val;
  let pred    = 0x0;
  let op_type = move_type.val;
  let attr    = 0x0;
  let rout    = 0x0;
}
multiclass MoveScalar<string asm,
                      MoveTypeEnumValue move_type> {
  defvar src_opnd = move_type.src_opnd;
  defvar dst_opnd = move_type.dst_opnd;
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstMoveScalar<inst_asm, dst_opnd, src_opnd, move_type>;
}

class InstMoveVector<string inst_asm, RegisterOperand dst_opnd, RegisterOperand src_opnd, MoveTypeEnumValue move_type> :
          InstStorage<(outs dst_opnd:$rin2), (ins src_opnd:$rin1, PRegOpndPrt:$pred, dst_opnd:$old),
                      !strconcat(inst_asm, " $rin2, $rin1   $pred"), // improper
                      []> { // maybe it's a bug
  let isMoveReg = 1;
  let Constraints = "$rin2 = $old"; // only support inplace overwrite

  let op_code = CmdType.Move.val;
  // let pred    = 0x0;
  let op_type = move_type.val;
  let attr    = 0x0;
  let rout    = 0x0;
}
multiclass MoveVector<string asm,
                      MoveTypeEnumValue move_type> {
  defvar src_opnd = move_type.src_opnd;
  defvar dst_opnd = move_type.dst_opnd;
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstMoveVector<inst_asm, dst_opnd, src_opnd, move_type>;
}
multiclass MaskedMoveVector<Instruction inst,
                            int elem_bits> {
  defvar vt = !cond(!eq(elem_bits, 1) : vi1, !eq(elem_bits, 32) : vi32);

  // only support integer type
  def : Pat<(vt:$rin2 (vselect vi1:$pred, vt:$rin1, vt:$old)),
            (vt:$rin2 (inst vt:$rin1, vi1:$pred, vt:$old))>;
}

class InstMoveImmScalar<string inst_asm, MoveTypeEnumValue move_type> : // only support i32
          InstMoveImm<(outs SRegOpnd:$rout), (ins i32imm:$value),
                       !strconcat(inst_asm, " $rout, $value"),
                       [(set i32:$rout, imm:$value)]>{
  // let isMoveImm = 1; // what happen ?? loss imm operand when enable
  let isReMaterializable = 1;

  let op_code = CmdType.Move_I.val;
  let pred    = 0x0;
  let op_type = move_type.val;
}
multiclass MoveImmScalar<string asm,
                         MoveTypeEnumValue move_type> {
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstMoveImmScalar<inst_asm, move_type>;
}

class InstMoveImmExtra<string inst_asm, MoveTypeEnumValue move_type> :
          InstMoveImmExtraFormat<(outs ERegOpnd:$rout), (ins i64imm:$value),
                                 !strconcat(inst_asm, " $rout, $value"),
                                 [(set i64:$rout, imm:$value)]>{ // todo restore this
  let isReMaterializable = 1;

  let op_code  = CmdType.Move_I.val;
  let pred     = 0x0;
  let op_type  = move_type.val;
}
multiclass MoveImmExtra<string asm,
                        MoveTypeEnumValue move_type> {
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstMoveImmExtra<inst_asm, move_type>;
}

class InstMoveImmSystem<string inst_asm, MoveTypeEnumValue move_type, SystemRegisterEnumValue sys_reg, Intrinsic intr> :
          InstMoveImm<(outs SRegOpnd:$rout), (ins),
                       !strconcat(inst_asm, " $rout, @", sys_reg.asm),
                       [(set i32:$rout, (intr))]>{
  let isReMaterializable = 1;

  let op_code = CmdType.Move_I.val;
  let pred    = 0x0;
  let op_type = move_type.val;
  let value   = sys_reg.val;
}
multiclass MoveImmSystem<string asm,
                         MoveTypeEnumValue move_type,
                         SystemRegisterEnumValue sys_reg,
                         Intrinsic intr> {
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm), "_", sys_reg.asm);

  def NAME # suffix : InstMoveImmSystem<inst_asm, move_type, sys_reg, intr>;
}

class InstSelectValue<string inst_asm, RegisterOperand opnd, MoveTypeEnumValue move_type, PredStateTypeEnumValue pred_state> :
          InstStorage<(outs opnd:$rout), (ins PRegOpnd:$pred, opnd:$rin1, opnd:$rin2),
                      !strconcat(inst_asm, " $rout, $pred, $rin1, $rin2"),
                      []> { // manual matching
  let op_code = CmdType.Select.val;
  // let pred    = 0x0;
  let op_type = move_type.val;
  let attr    = pred_state.val;
}
multiclass SelectValueScalar<string asm,
                             PatFrag mask_node,
                             MoveTypeEnumValue move_type,
                             PredStateTypeEnumValue pred_state> {
  defvar inst_asm = !strconcat(asm, move_type.asm, pred_state.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSelectValue<inst_asm, SRegOpnd, move_type, pred_state>;

  foreach vt = [i32, f32, f16] in {
    // manual match
    def : Pat<(vt:$rout (select (mask_node vi1:$pred), vt:$rin1, vt:$rin2)),
              (vt:$rout (!cast<Instruction>(NAME # suffix) vi1:$pred, vt:$rin1, vt:$rin2))>;
    // match i1 condition
    if !eq(pred_state, PredStateType.State_Any) then {
      def : Pat<(vt:$rout (select i1:$pred, vt:$rin1, vt:$rin2)),
                (vt:$rout (!cast<Instruction>(NAME # suffix) i1:$pred, vt:$rin1, vt:$rin2))>;
    }
  }
}
multiclass SelectValueExtra<string asm,
                            PatFrag mask_node,
                            MoveTypeEnumValue move_type,
                            PredStateTypeEnumValue pred_state> {
  defvar vt = i64;
  defvar inst_asm = !strconcat(asm, move_type.asm, pred_state.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSelectValue<inst_asm, ERegOpnd, move_type, pred_state>;

  // manual match
  def : Pat<(vt:$rout (select (mask_node vi1:$pred), vt:$rin1, vt:$rin2)),
            (vt:$rout (!cast<Instruction>(NAME # suffix) vi1:$pred, vt:$rin1, vt:$rin2))>;
  // match i1 condition
  if !eq(pred_state, PredStateType.State_Any) then {
    def : Pat<(vt:$rout (select i1:$pred, vt:$rin1, vt:$rin2)),
              (vt:$rout (!cast<Instruction>(NAME # suffix) i1:$pred, vt:$rin1, vt:$rin2))>;
  }
}
multiclass SelectValueVector<string asm,
                             MoveTypeEnumValue move_type> {
  defvar inst_asm = !strconcat(asm, move_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstSelectValue<inst_asm, VRegOpnd, move_type, PredStateType.None>;

  foreach vt = [vi32, vf32, vf16] in {
    // manual match
    def : Pat<(vt:$rout (vselect vi1:$pred, vt:$rin1, vt:$rin2)),
              (vt:$rout (!cast<Instruction>(NAME # suffix) vi1:$pred, vt:$rin1, vt:$rin2))>;
    // match i1 condition
    def : Pat<(vt:$rout (select i1:$pred, vt:$rin1, vt:$rin2)), // SDNode is select
              (vt:$rout (!cast<Instruction>(NAME # suffix) i1:$pred, vt:$rin1, vt:$rin2))>;
  }
}

class InstJmp<string inst_asm, PredStateTypeEnumValue pred_state, list<dag> pattern> :
          InstSpecial<(outs), (ins PRegOpnd:$pred, brtarget:$value),
                      !strconcat(inst_asm, " $pred, $value"),
                      pattern> {
  let isBranch     = 1;
  let isBarrier    = 1;
  let isTerminator = 1;

  let op_code = CmdType.Jmp.val;
  let dtype   = DataType.None.val;
  let attr    = pred_state.val;
}

class InstLoop : InstLoopFormat<(outs), (ins i8imm:$depth, SRegOpnd:$lpc, brtarget:$lpend),
                                "loop.$depth $lpc, $lpend", []> {
  let hasSideEffects = 1;
  // let isBranch     = 1;
  // let isBarrier    = 1;
  let isTerminator = 1;

  let op_code = CmdType.Loop.val;
}

class InstReturn : InstSpecial<(outs), (ins),
                               "ret", [(ISDRetFlag)]> {
  // let isBranch     = 1;
  let isReturn     = 1;
  let isBarrier    = 1;
  let isTerminator = 1;

  let op_code = CmdType.Ret.val;
  let pred    = 0x0;
  let dtype   = DataType.None.val;
  let attr    = 0x0;
  let value   = 0x0;
}

class InstSyncThreads<SyncScopeEnumValue scope> : InstSpecial<(outs), (ins),
                                                  "sync", [(int_drai_sync_threads (i32 scope.val))]> {
  let hasSideEffects = 1;
  let mayLoad        = 1;
  let mayStore       = 1;
  let isConvergent   = 1;

  let op_code = CmdType.SyncThreads.val;
  let pred    = 0x0;
  let dtype   = 0x0;
  let attr    = scope.val;
  let value   = 0x0;
}

class InstMemoryFence<FenceScopeEnumValue scope> : InstSpecial<(outs), (ins),
                                                  "fence", [(int_drai_memory_fence (i32 scope.val))]> {
  let hasSideEffects = 1;
  let mayLoad        = 1;
  let mayStore       = 1;

  let op_code = CmdType.MemoryFence.val;
  let pred    = 0x0;
  let dtype   = 0x0;
  let attr    = scope.val;
  let value   = 0x0;
}

class InstPrint<string inst_asm, RegisterOperand opnd, Intrinsic intr, ValueType vt, RegisterTypeEnumValue reg_type, DataTypeEnumValue data_type> :
          InstCModel<(outs), (ins opnd:$reg),
                     !strconcat(inst_asm, " $reg"),
                     [(intr vt:$reg)]>{
  let hasSideEffects = 1;
  let mayLoad        = 1;
  let mayStore       = 1;

  let op_code = CmdType.Cmodel.val;
  let pred    = 0x0;
  let dtype   = data_type.val;
  let attr    = !or(!cast<bits<4>>(CModelOpType.PrintReg.val), !shl(!cast<bits<4>>(reg_type.val), 4));
  let value   = 0x0;
}
multiclass PrintValue<string asm,
                      Intrinsic intr,
                      RegisterTypeEnumValue reg_type,
                      DataTypeEnumValue data_type> {
  defvar vt = !cond(
      !eq(reg_type, RegisterType.Scalar) : data_type.vt_s,
      !eq(reg_type, RegisterType.EScalar) : data_type.vt_s,
      !eq(reg_type, RegisterType.Vector) : data_type.vt_v,
      !eq(reg_type, RegisterType.Pred) : vi1
  );
  defvar opnd = reg_type.opnd;
  defvar inst_asm = !strconcat(asm, reg_type.asm, data_type.asm);
  defvar suffix = !strconcat("_", !subst(".", "_", inst_asm));

  def NAME # suffix : InstPrint<inst_asm, opnd, intr, vt, reg_type, data_type>;
}