class DRAIReg<bits<6> Enc, string n> : Register<n> {
  let HWEncoding{15-6} = 0;
  let HWEncoding{5-0} = Enc;
  let Namespace = "DRAI";
  bits<6> enc = Enc;
}

class ScalarReg<bits<6> Enc, string n> : DRAIReg<Enc, n>;
class ExtraReg<bits<6> Enc, string n> : DRAIReg<Enc, n>; // 64bit register
class VectorReg<bits<6> Enc, string n> : DRAIReg<Enc, n>;
class PredReg<bits<6> Enc, string n> : DRAIReg<Enc, n>;

foreach I = 0-63 in {
  def SR#I : ScalarReg<I, "s"#I>, DwarfRegNum<[!add(I, 0)]>;
}

foreach I = 0-63 in {
  if !eq(I, 63) then {
    def ArgBase : ExtraReg<I, "arg">, DwarfRegNum<[!add(I, 64)]>;
  } else {
    def ER#I : ExtraReg<I, "e"#I>, DwarfRegNum<[!add(I, 64)]>;
  }
}

foreach I = 0-63 in {
  def VR#I : VectorReg<I, "v"#I>, DwarfRegNum<[!add(I, 128)]>;
}

foreach I = 0-63 in {
  if !eq(I, 0) then {
    def PredTruth : PredReg<I, "pt">, DwarfRegNum<[!add(I, 192)]>;
  } else {
    def PR#I : PredReg<I, "p"#I>, DwarfRegNum<[!add(I, 192)]>;
  }
}

def PseudoR : DRAIReg<0, "pseudo">, DwarfRegNum<[2333]>;

// addRegisterClass!!!
def SReg : RegisterClass<"DRAI", [i32, f16, f32], 32, (add (sequence "SR%u", 0, 63))>;
def EReg : RegisterClass<"DRAI", [i64], 64, (add (sequence "ER%u", 0, 62), ArgBase)>;// ArgBase as an EReg is allowed to use
def VReg : RegisterClass<"DRAI", [vi32, vf16, vf32], !mul(simd_width, 32), (add (sequence "VR%u", 0, 63))>;
def PReg : RegisterClass<"DRAI", [i1, vi1], simd_width, (add PredTruth, (sequence "PR%u", 1, 63))> { // need increasing and continuous
  let AltOrders = [(sub PReg, PredTruth)]; // not allocatable
  let AltOrderSelect = [{ return true; }];
}
def PseudoReg : RegisterClass<"DRAI", [i1, i32, i64, vi1, vi32], 0, (add PseudoR)>;

def SRegOpnd : RegisterOperand<SReg>;
def ERegOpnd : RegisterOperand<EReg>;
def VRegOpnd : RegisterOperand<VReg>;
def PRegOpnd : RegisterOperand<PReg>;
def PseudoOpnd : RegisterOperand<PseudoReg>;

def PRegOpndPrt : RegisterOperand<PReg, "printPredicate">;