include "llvm/Target/Target.td"

defvar simd_width = 64;
defvar vi32 = v64i32;
defvar vi64 = v64i64;
defvar vf16 = v64f16;
defvar vf32 = v64f32;
defvar vi1  = v64i1;
defvar im_ty = i64; // integer mask type

include "DRAIRegisterInfo.td"
include "DRAITypeDef.td"
include "DRAISchedule.td"
include "DRAICallingConv.td"
include "DRAIInstrFormats.td"
include "DRAIInstrInfo.td"

def : ProcessorModel<"drai001", DRAISchedModel, []>;

def DRAIInstrInfo : InstrInfo;

def DRAIAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
//   int PassSubtarget = 1;
//   int Variant = 0;
}

def DRAIAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

def DRAIAsmParserVariant : AsmParserVariant {
  int Variant = 0;

  // Recognize hard coded registers.
  string RegisterPrefix = "%";
}

def DRAI : Target {
  let InstructionSet = DRAIInstrInfo;
  let AssemblyWriters = [DRAIAsmWriter];
  let AssemblyParsers = [DRAIAsmParser];
  let AssemblyParserVariants = [DRAIAsmParserVariant];
  let AllowRegisterRenaming = 1;
}