//===-- DRAIMCInstLower.cpp - Lower MachineInstr to MCInst ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DRAIMCInstLower.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/bit.h"

using namespace llvm;

// Return the VK_* enumeration for MachineOperand target flags Flags.
static MCSymbolRefExpr::VariantKind getVariantKind(unsigned Flags) {
  // TODO Implement
  return MCSymbolRefExpr::VK_None;
}

DRAIMCInstLower::DRAIMCInstLower(MCContext &Ctx, AsmPrinter &Printer)
    : Ctx(Ctx), Printer(Printer) {}

const MCExpr *
DRAIMCInstLower::getExpr(const MachineOperand &MO,
                         MCSymbolRefExpr::VariantKind Kind) const {
  const MCSymbol *Symbol;
  bool HasOffset = true;
  switch (MO.getType()) {
  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB()->getSymbol();
    HasOffset = false;
    break;

  case MachineOperand::MO_GlobalAddress:
    Symbol = Printer.getSymbol(MO.getGlobal());
    break;

  case MachineOperand::MO_ExternalSymbol:
    Symbol = Printer.GetExternalSymbolSymbol(MO.getSymbolName());
    break;

  case MachineOperand::MO_JumpTableIndex:
    Symbol = Printer.GetJTISymbol(MO.getIndex());
    HasOffset = false;
    break;

  case MachineOperand::MO_ConstantPoolIndex:
    Symbol = Printer.GetCPISymbol(MO.getIndex());
    break;

  case MachineOperand::MO_BlockAddress:
    Symbol = Printer.GetBlockAddressSymbol(MO.getBlockAddress());
    break;

  default:
    llvm::dbgs() << MO.getType() << "\n";
    llvm_unreachable("unknown operand type");
  }
  const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, Kind, Ctx);
  if (HasOffset)
    if (int64_t Offset = MO.getOffset()) {
      const MCExpr *OffsetExpr = MCConstantExpr::create(Offset, Ctx);
      Expr = MCBinaryExpr::createAdd(Expr, OffsetExpr, Ctx);
    }
  return Expr;
}

MCOperand DRAIMCInstLower::lowerOperand(const MachineOperand &MO) const {
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    return MCOperand::createReg(MO.getReg());

  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm());

  case MachineOperand::MO_FPImmediate: {
    APFloat Val = MO.getFPImm()->getValueAPF();
    return MCOperand::createSFPImm(bit_cast<uint32_t>(Val.convertToFloat()));
  }

  default: {
    MCSymbolRefExpr::VariantKind Kind = getVariantKind(MO.getTargetFlags());
    return MCOperand::createExpr(getExpr(MO, Kind));
  }
  }
}

void DRAIMCInstLower::lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());
  for (unsigned I = 0, E = MI->getNumOperands(); I != E; ++I) {
    const MachineOperand &MO = MI->getOperand(I);
    // Ignore all implicit register operands.
    if (!MO.isReg() || !MO.isImplicit())
      OutMI.addOperand(lowerOperand(MO));
  }
}
