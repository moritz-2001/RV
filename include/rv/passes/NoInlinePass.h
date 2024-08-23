#ifndef NOINLINEPASS_H
#define NOINLINEPASS_H

#include "llvm/IR/PassManager.h"

class NoInlinePass : public llvm::PassInfoMixin<NoInlinePass> {
public:
  NoInlinePass() {}

  static llvm::StringRef name() { return "rv::NoInlinePass"; }
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &) {
    if (F.hasFnAttribute("iskernel")) {
      F.addFnAttr(llvm::Attribute::NoInline);
    }
    return llvm::PreservedAnalyses::all();
  }
};

#endif //NOINLINEPASS_H
