//===----------- JITSymbol.cpp - JITSymbol class implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// JITSymbol class implementation plus helper functions.
//
//===----------------------------------------------------------------------===//

#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Object/SymbolicFile.h"

using namespace llvm;

JITSymbolFlags llvm::JITSymbolFlags::fromGlobalValue(const GlobalValue &GV) {
  JITSymbolFlags Flags = JITSymbolFlags::None;
  if (GV.hasWeakLinkage() || GV.hasLinkOnceLinkage())
    Flags |= JITSymbolFlags::Weak;
  if (GV.hasCommonLinkage())
    Flags |= JITSymbolFlags::Common;
  if (!GV.hasLocalLinkage() && !GV.hasHiddenVisibility())
    Flags |= JITSymbolFlags::Exported;
  return Flags;
}

JITSymbolFlags
llvm::JITSymbolFlags::fromObjectSymbol(const object::BasicSymbolRef &Symbol) {
  JITSymbolFlags Flags = JITSymbolFlags::None;
  if (Symbol.getFlags() & object::BasicSymbolRef::SF_Weak)
    Flags |= JITSymbolFlags::Weak;
  if (Symbol.getFlags() & object::BasicSymbolRef::SF_Common)
    Flags |= JITSymbolFlags::Common;
  if (Symbol.getFlags() & object::BasicSymbolRef::SF_Exported)
    Flags |= JITSymbolFlags::Exported;
  return Flags;
}

ARMJITSymbolFlags llvm::ARMJITSymbolFlags::fromObjectSymbol(
                                         const object::BasicSymbolRef &Symbol) {
  ARMJITSymbolFlags Flags;
  if (Symbol.getFlags() & object::BasicSymbolRef::SF_Thumb)
    Flags |= ARMJITSymbolFlags::Thumb;
  return Flags;
}

/// Performs lookup by, for each symbol, first calling
///        findSymbolInLogicalDylib and if that fails calling
///        findSymbol.
Expected<JITSymbolResolver::LookupResult>
LegacyJITSymbolResolver::lookup(const LookupSet &Symbols) {
  MY_DEBUG(my_dbgs() << "LegacyJITSymbolResolver::lookup" << "\n");

  std::set<std::string> errSymbols;
//  JITSymbolResolver::LookupResult errResult;
  
  JITSymbolResolver::LookupResult Result;
  for (auto &Symbol : Symbols) {
    std::string SymName = Symbol.str();
    MY_DEBUG(my_dbgs() << " find SymName=" << SymName << "\n");

    if (auto Sym = findSymbolInLogicalDylib(SymName)) {
      MY_DEBUG(my_dbgs() << "  found in dylib" << "\n");

      if (auto AddrOrErr = Sym.getAddress()) {
        MY_DEBUG(my_dbgs() << "   has address" << "\n");
        Result[Symbol] = JITEvaluatedSymbol(*AddrOrErr, Sym.getFlags());
      }
      else {
        MY_DEBUG(my_dbgs() << "   no address error" << "\n");
        errSymbols.insert(SymName);
//        errResult = AddrOrErr.takeError();
//        return AddrOrErr.takeError();
      }
    } else if (auto Err = Sym.takeError()) {
      MY_DEBUG(my_dbgs() << " Sym.takeError when findSymbolInLogicalDylib" << "\n");
      errSymbols.insert(SymName);
//      errResult = std::move(Err);
//      return std::move(Err);
    } else {
      MY_DEBUG(my_dbgs() << " not found and no Sym.takeError" << "\n");
      // findSymbolInLogicalDylib failed. Lets try findSymbol.
      if (auto Sym = findSymbol(SymName)) {
        MY_DEBUG(my_dbgs() << "  findSymbol success" << "\n");
        if (auto AddrOrErr = Sym.getAddress()) {
          MY_DEBUG(my_dbgs() << "   has address" << "\n");
          Result[Symbol] = JITEvaluatedSymbol(*AddrOrErr, Sym.getFlags());
        } else {
          MY_DEBUG(my_dbgs() << "   no address error" << "\n");
          errSymbols.insert(SymName);
//          errResult = AddrOrErr.takeError();
//          return AddrOrErr.takeError();
        }
      } else if (auto Err = Sym.takeError()) {
        MY_DEBUG(my_dbgs() << "   Sym.takeError when findSymbol" << "\n");
        errSymbols.insert(SymName);
//        errResult = std::move(Err);
//        return std::move(Err);
      } else {
        MY_DEBUG(my_dbgs() << "  findSymbol failed and no Sym.takeError" << "\n");
        errSymbols.insert(SymName);
//        errResult = make_error<StringError>("Symbol not found: " + Symbol,
//                                       inconvertibleErrorCode());
//        return make_error<StringError>("Symbol not found: " + Symbol,
//                                       inconvertibleErrorCode());
      }
    }
  }

  if (!errSymbols.empty()) {
      for (auto & val : errSymbols) {
          MY_DEBUG(my_dbgs() << " error symbol: " << val << "\n");
      }
      return make_error<StringError>("Symbol not found", inconvertibleErrorCode());
  }

  return std::move(Result);
}

/// Performs flags lookup by calling findSymbolInLogicalDylib and
///        returning the flags value for that symbol.
Expected<JITSymbolResolver::LookupFlagsResult>
LegacyJITSymbolResolver::lookupFlags(const LookupSet &Symbols) {
  JITSymbolResolver::LookupFlagsResult Result;

  for (auto &Symbol : Symbols) {
    std::string SymName = Symbol.str();
    if (auto Sym = findSymbolInLogicalDylib(SymName))
      Result[Symbol] = Sym.getFlags();
    else if (auto Err = Sym.takeError())
      return std::move(Err);
  }

  return std::move(Result);
}
