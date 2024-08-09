#ifndef PRINTPASS_H
#define PRINTPASS_H

#include "rv/legacy/passes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include <llvm/IR/Verifier.h>

class PrintPass : public llvm::PassInfoMixin<PrintPass> {
public:
  PrintPass() {}

  static llvm::StringRef name() { return "rv::PrintPass"; }
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &) {
    if (F.hasFnAttribute("iskernel")) {
      F.viewCFG();
    }
    return llvm::PreservedAnalyses::all();
  }
};

#endif //PRINTPASS_H
