//===--- DRAI.cpp - DRAI ToolChain Implementations ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DRAI.h"
#include "Arch/ARM.h"
#include "Arch/Mips.h"
#include "Arch/Sparc.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

void drai::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const ToolChain &TC = getToolChain();
  std::string Linker = TC.GetProgramPath(getShortName());
  ArgStringList CmdArgs;

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  AddLinkerInputs(TC, Inputs, Args, CmdArgs, JA);
  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs))
    AddRunTimeLibs(TC, TC.getDriver(), CmdArgs, Args);

  const char *Exec = Args.MakeArgString(getToolChain().GetProgramPath("ld.lld"));
  // C.addCommand(std::make_unique<Command>(JA, *this, Exec, CmdArgs, Inputs));
  C.addCommand(std::make_unique<Command>(
      JA, *this, ResponseFileSupport::AtFileCurCP(), Exec, // Args.MakeArgString(Linker),
      CmdArgs, Inputs)); //, Output));
}

DRAIToolChain::DRAIToolChain(const Driver &D, const llvm::Triple &Triple,
                               const llvm::opt::ArgList &Args)
	:	ToolChain(D, Triple, Args)
{
  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);
}

DRAIToolChain::~DRAIToolChain()
{
}

ToolChain::RuntimeLibType
DRAIToolChain::GetRuntimeLibType(const llvm::opt::ArgList &Args) const
{
  if (Arg *A = Args.getLastArg(clang::driver::options::OPT_rtlib_EQ)) {
    StringRef Value = A->getValue();
    if (Value != "compiler-rt")
      getDriver().Diag(clang::diag::err_drv_invalid_rtlib_name)
          << A->getAsString(Args);
  }

  return ToolChain::RLT_CompilerRT;
}

bool DRAIToolChain::IsIntegratedAssemblerDefault() const
{
  return true;
}

bool DRAIToolChain::isPICDefault() const
{
  return false;
}

bool DRAIToolChain::isPIEDefault(const llvm::opt::ArgList &Args) const
{
  return false;
}

bool DRAIToolChain::isPICDefaultForced() const
{
  return false;
}

void DRAIToolChain::addClangTargetOptions(const ArgList &DriverArgs,
                                  ArgStringList &CC1Args,
                                  Action::OffloadKind DeviceOffloadKind) const {
  if (!DriverArgs.hasArg(options::OPT_fdrai_host_only)) {
    llvm::dbgs() << "cc1 enable fdrai-is-device!!!\n";
    CC1Args.push_back("-fdrai-is-device");
  }

  CC1Args.push_back("-fsimd-branch");

  // CC1Args.push_back("-nostdsysteminc");
  // if (DriverArgs.hasFlag(options::OPT_fuse_init_array,
  //                        options::OPT_fno_use_init_array,
  //                        true))
  // {
  //   CC1Args.push_back("-fuse-init-array");
  // }
}

Tool *DRAIToolChain::buildLinker() const {
  return new tools::drai::Linker(*this);
}


