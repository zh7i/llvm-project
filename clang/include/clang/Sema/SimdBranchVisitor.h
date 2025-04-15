#pragma once

#include "clang/AST/Stmt.h"
#include "clang/Sema/Sema.h"
#include <vector>

using namespace clang;

/*
deduce simd branch stmts
loop condition is vector -> loop is simd
if condition is vector   -> if is simd
loop is simd             -> body break/continue is simd
if of loop body is simd  -> then/else break/continue is simd

disable simd break/continue in scalar loop
*/
class SimdBranchVisitor {
public:
  SimdBranchVisitor(Sema &sema) : sema_(sema) {}

  void Visit(Stmt *stmt) {
    if (stmt == nullptr) {
      return;
    }

    switch (stmt->getStmtClass()) {
    case Stmt::ForStmtClass:
      VisitLoop(cast<ForStmt>(stmt));
      return;
    case Stmt::DoStmtClass:
      VisitLoop(cast<DoStmt>(stmt));
      return;
    case Stmt::IfStmtClass:
      VisitBranch(cast<IfStmt>(stmt));
      return;
    case Stmt::BreakStmtClass:
      VisitLoopCtrl(cast<BreakStmt>(stmt));
      return;
    case Stmt::ContinueStmtClass:
      VisitLoopCtrl(cast<ContinueStmt>(stmt));
      return;
    default:
      break;
    }

    for (Stmt *child : stmt->children()) {
      Visit(child);
    }
  }

private:
  template <typename LP> void VisitLoop(LP *stmt) {
    Expr *cond = stmt->getCond();
    Stmt *body = stmt->getBody();
    bool is_simd = cond && cond->getType()->isVectorType(); // maybe uncondition
    SetSimdBranch(stmt, is_simd);
    stmts_.push_back(stmt);
    Visit(body);
    stmts_.pop_back();
  }

  void VisitBranch(IfStmt *stmt) {
    bool is_simd = stmt->getCond()->getType()->isVectorType();
    SetSimdBranch(stmt, is_simd);
    if (is_simd) {
      stmts_.push_back(stmt); // only push simd if
    }
    Visit(stmt->getThen());
    if (stmt->getElse()) {
      Visit(stmt->getElse());
    }
    if (is_simd) {
      stmts_.pop_back();
    }
  }

  template <typename LC> void VisitLoopCtrl(LC *stmt) {
    if (!stmts_.empty() && IsSimdBranch(stmts_.back())) {
      SetSimdBranch(stmt, true);
      for (int32_t i = stmts_.size() - 1; i >= 0; --i) {
        if (stmts_[i]->getStmtClass() == Stmt::IfStmtClass) {
          continue;
        }
        if (!IsSimdBranch(stmts_[i])) {
          sema_.Diag(stmt->getBeginLoc(),
                     diag::err_simd_break_continue_in_scalar_loop)
              << stmt->getSourceRange();
          sema_.Diag(stmts_[i]->getBeginLoc(), diag::note_upgrade_loop_cond)
              << stmts_[i]->getSourceRange();
        }
        break;
      }
    }
  }

  bool IsSimdBranch(Stmt *stmt) {
    switch (stmt->getStmtClass()) {
    case Stmt::ForStmtClass:
      return cast<ForStmt>(stmt)->IsSimdBranch();
    case Stmt::DoStmtClass:
      return cast<DoStmt>(stmt)->IsSimdBranch();
    case Stmt::IfStmtClass:
      return cast<IfStmt>(stmt)->IsSimdBranch();
    case Stmt::BreakStmtClass:
      return cast<BreakStmt>(stmt)->IsSimdBranch();
    case Stmt::ContinueStmtClass:
      return cast<ContinueStmt>(stmt)->IsSimdBranch();
    default:
      llvm_unreachable("unexpect");
      return false;
    }
  }

  void SetSimdBranch(Stmt *stmt, bool simd_branch) {
    if (simd_branch && !IsSimdBranch(stmt)) {
      sema_.Diag(stmt->getBeginLoc(), diag::simd_branch_defined_here);
    }
    switch (stmt->getStmtClass()) {
    case Stmt::ForStmtClass:
      cast<ForStmt>(stmt)->SetSimdBranch(simd_branch);
      break;
    case Stmt::DoStmtClass:
      cast<DoStmt>(stmt)->SetSimdBranch(simd_branch);
      break;
    case Stmt::IfStmtClass:
      cast<IfStmt>(stmt)->SetSimdBranch(simd_branch);
      break;
    case Stmt::BreakStmtClass:
      cast<BreakStmt>(stmt)->SetSimdBranch(simd_branch);
      break;
    case Stmt::ContinueStmtClass:
      cast<ContinueStmt>(stmt)->SetSimdBranch(simd_branch);
      break;
    default:
      llvm_unreachable("unexpect");
    }
  }

  Sema &sema_;
  std::vector<Stmt *> stmts_; // loop & simd if stmts
};