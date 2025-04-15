#include "DRAI.h"

#include "DRAIRegisterInfo.h"
#include "DRAISubtarget.h"
#include "MCTargetDesc/DRAIMCTargetDesc.h"
#include "TargetInfo/DRAITargetInfo.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/SMLoc.h"

using namespace llvm;

#define DEBUG_TYPE "drai-asm-parser"

namespace {
class DRAIAsmParser : public MCTargetAsmParser {
  MCAsmParser &Parser;

#define GET_ASSEMBLER_HEADER
#include "DRAIGenAsmMatcher.inc"

  enum ParseStatus {
    ParseSucc = 0,
    ParseFail = 1,
  };

  OperandMatchResultTy tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;

  bool parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                     SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool ParseDirective(AsmToken DirectiveID) override;

  // helper functions
  bool ParseOperand(OperandVector &Operands, StringRef Mnemonic);

  bool tryParseRegisterOperand(OperandVector &Operands, StringRef Mnemonic);

  std::optional<unsigned> getClassReg(unsigned RC, unsigned RegNo);

  std::optional<MCRegister> matchRegisterName(StringRef Name);

public:
  DRAIAsmParser(const MCSubtargetInfo &STI, MCAsmParser &parser,
                const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII), Parser(parser) {
    setAvailableFeatures(ComputeAvailableFeatures(getSTI().getFeatureBits()));
  }

  MCAsmParser &getParser() const { return Parser; }
  MCAsmLexer &getLexer() const { return Parser.getLexer(); }
};

class DRAIOperand : public MCParsedAsmOperand {
  enum KindTy { k_Immediate, k_Memory, k_Register, k_Token } Kind;

  struct Token {
    const char *Data;
    unsigned Length;
  };
  struct PhysRegOp {
    unsigned RegNum;
  };
  struct ImmOp {
    const MCExpr *Val;
  };
  struct MemOp {
    unsigned Base;
    const MCExpr *Off;
  };

  union {
    struct Token Tok;
    struct PhysRegOp Reg;
    struct ImmOp Imm;
    struct MemOp Mem;
  };

  SMLoc StartLoc, EndLoc;

public:
  DRAIOperand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

  static std::unique_ptr<DRAIOperand> CreateToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<DRAIOperand>(k_Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<DRAIOperand> CreateReg(unsigned RegNum, SMLoc S,
                                                SMLoc E) {
    auto Op = std::make_unique<DRAIOperand>(k_Register);
    Op->Reg.RegNum = RegNum;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<DRAIOperand> CreateImm(const MCExpr *Val, SMLoc S,
                                                SMLoc E) {
    auto Op = std::make_unique<DRAIOperand>(k_Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<DRAIOperand>
  CreateMem(unsigned Base, const MCExpr *Off, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<DRAIOperand>(k_Memory);
    Op->Mem.Base = Base;
    Op->Mem.Off = Off;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  bool isReg() const override { return Kind == k_Register; }
  bool isImm() const override { return Kind == k_Immediate; }
  bool isToken() const override { return Kind == k_Token; }
  bool isMem() const override { return Kind == k_Memory; }

  StringRef getToken() const {
    assert(Kind == k_Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  unsigned getReg() const override {
    assert(Kind == k_Register && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == k_Immediate && "Invalid access!");
    return Imm.Val;
  }

  unsigned getMemBase() const {
    assert(Kind == k_Memory && "Invalid access!");
    return Mem.Base;
  }

  const MCExpr *getMemOff() const {
    assert(Kind == k_Memory && "Invalid access!");
    return Mem.Off;
  }

  SMLoc getStartLoc() const override { return StartLoc; }
  SMLoc getEndLoc() const override { return EndLoc; }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case k_Immediate:
      OS << "Imm<" << *getImm() << ">";
      break;
    case k_Memory:
      OS << "Mem<";
      OS << getMemBase();
      OS << ", ";
      OS << *getMemOff();
      OS << ">";
      break;
    case k_Register:
      OS << "Register<" << getReg() << ">";
      break;
    case k_Token:
      OS << getToken();
      break;
    }
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediate when possible. Null MCExpr = 0.
    if (Expr == 0) {
      llvm_unreachable("unexpect");
      // Inst.addOperand(MCOperand::createImm(0));
    } else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr)) {
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    } else {
      Inst.addOperand(MCOperand::createExpr(Expr));
    }
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCExpr *Expr = getImm();
    addExpr(Inst, Expr);
  }

  void addMemOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getMemBase()));

    const MCExpr *Expr = getMemOff();
    addExpr(Inst, Expr);
  }
};

} // namespace

bool DRAIAsmParser::parseRegister(MCRegister &RegNo, SMLoc &StartLoc,
                                  SMLoc &EndLoc) {
  return tryParseRegister(RegNo, StartLoc, EndLoc) != MatchOperand_Success;
}

OperandMatchResultTy DRAIAsmParser::tryParseRegister(MCRegister &Reg,
                                                     SMLoc &StartLoc,
                                                     SMLoc &EndLoc) {
  const AsmToken &Tok = Parser.getTok();

  if (Tok.is(AsmToken::Identifier)) {
    if (auto r = matchRegisterName(Tok.getString())) {
      Reg = r.value();
      return MatchOperand_Success;
    }
  } else if (Tok.is(AsmToken::Integer)) {
    // RegNum = matchRegisterByNumber(static_cast<unsigned>(Tok.getIntVal()),
    //                                Mnemonic.lower());
    llvm_unreachable("todo impl");
    return MatchOperand_Success;
  }

  return MatchOperand_ParseFail;
}

bool DRAIAsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                     SMLoc NameLoc, OperandVector &Operands) {
  Operands.push_back(DRAIOperand::CreateToken(Name, NameLoc)); // ????

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    do {
      if (ParseOperand(Operands, Name) == ParseFail) {
        SMLoc Loc = getLexer().getLoc();
        Parser.eatToEndOfStatement();
        return Error(Loc, "unexpected token in argument list");
      }
    } while (getLexer().is(AsmToken::Comma) &&
             Parser.Lex().isNot(AsmToken::Error)); // eat the comma
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    Parser.eatToEndOfStatement();
    return Error(Loc, "unexpected token in argument list");
  }

  Parser.Lex(); // eat the EndOfStatement
  return ParseSucc;
}

bool DRAIAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                            OperandVector &Operands,
                                            MCStreamer &Out,
                                            uint64_t &ErrorInfo,
                                            bool MatchingInlineAsm) {
  MCInst Inst;
  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);
  switch (MatchResult) {
  default:
    break;
  case Match_Success: {
    // todo expand pseudo instruction
    Inst.setLoc(IDLoc);
    Out.emitInstruction(Inst, *STI);
    return ParseSucc;
  }
  case Match_MissingFeature:
    return Error(IDLoc, "instruction required feature not currently enabled");
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size()) {
        return Error(IDLoc, "too few operands for instruction");
      }
      ErrorLoc = Operands[ErrorInfo]->getStartLoc();
      if (ErrorLoc == SMLoc()) {
        ErrorLoc = IDLoc;
      }
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_MnemonicFail:
    return Error(IDLoc, "invalid instruction");
  }
  return ParseFail;
}

bool DRAIAsmParser::ParseDirective(AsmToken DirectiveID) {
  llvm_unreachable("todo impl");
  return false;
}

bool DRAIAsmParser::ParseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // need this for "let ParserMethod = 1" in tablegen;
  // OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);
  // if (ResTy == MatchOperand_Success)
  //   return ParseSucc;
  // if (ResTy == MatchOperand_ParseFail)
  //   return ParseFail;

  switch (getLexer().getKind()) {
  default:
    return Error(Parser.getTok().getLoc(), "unexpected token in opernad");
  case AsmToken::Percent: {
    // SMLoc Start = Parser.getTok().getLoc();
    Parser.Lex(); // eat percent token

    if (tryParseRegisterOperand(Operands, Mnemonic) == ParseSucc) {
      if (getLexer().is(AsmToken::LParen)) {
        llvm_unreachable("todo impl");
      }
      return ParseSucc;
    }
    llvm_unreachable("todo impl");
  }
  }

  return ParseFail;
}

bool DRAIAsmParser::tryParseRegisterOperand(OperandVector &Operands,
                                            StringRef Mnemonic) {
  SMLoc Start = Parser.getTok().getLoc();
  SMLoc End = Parser.getTok().getEndLoc();

  MCRegister Reg;
  if (tryParseRegister(Reg, Start, End) == MatchOperand_ParseFail) {
    return ParseFail;
  }

  Operands.push_back(DRAIOperand::CreateReg((unsigned)Reg, Start, End));
  Parser.Lex(); // eat register token
  return ParseSucc;
}

std::optional<unsigned> DRAIAsmParser::getClassReg(unsigned RC,
                                                   unsigned RegNo) {
  const MCRegisterInfo *RegInfo = this->getContext().getRegisterInfo();
  const MCRegisterClass &RegClass = RegInfo->getRegClass(RC);
  if (RegNo >= RegClass.getNumRegs()) {
    return std::nullopt;
  }
  return *(RegClass.begin() + RegNo);
}

std::optional<MCRegister> DRAIAsmParser::matchRegisterName(StringRef Name) {
  if (Name.size() < 2) {
    return std::nullopt;
  }
  APInt idx;
  StringRef suffix = Name.slice(1, StringRef::npos);
  if (suffix.getAsInteger(10, idx)) {
    return std::nullopt;
  }

  switch (Name.front()) {
  case 's':
    return getClassReg(DRAI::SRegRegClassID, idx.getZExtValue());
  case 'e':
    return getClassReg(DRAI::ERegRegClassID, idx.getZExtValue());
  case 'v':
    return getClassReg(DRAI::VRegRegClassID, idx.getZExtValue());
  case 'p':
    return getClassReg(DRAI::PRegRegClassID, idx.getZExtValue());
  default:
    break;
  }

  return std::nullopt;
}

extern "C" void LLVMInitializeDRAIAsmParser() {
  RegisterMCAsmParser<DRAIAsmParser> X(getTheDRAITarget());
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "DRAIGenAsmMatcher.inc"