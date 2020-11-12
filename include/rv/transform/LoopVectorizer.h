//===- rv/transform/loopVectorizer.h - loop vectorizer pass  --*- C++ -*-===//
//
// Part of the RV Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//



#include "llvm/Pass.h"

#include "llvm/Transforms/Utils/ValueMapper.h"
#include "rv/transform/remTransform.h"
#include "rv/analysis/reductionAnalysis.h"
#include "rv/analysis/loopAnnotations.h"
#include "rv/config.h"
#include "llvm/IR/PassManager.h"
#include "rv/transform/remTransform.h"
#include "rv/rv.h"
#include "rv/passes.h"

namespace llvm {
  class Loop;
  class LoopInfo;
  class DominatorTree;
  class ScalarEvolution;
  class PostDominatorTree;
  class MemoryDependenceResults;
  class BranchProbabilityInfo;
  class LoopDependenceInfo;
}


namespace rv {

class LoopVectorizer : public llvm::FunctionPass {
public:
  static char ID;
  LoopVectorizer()
  : llvm::FunctionPass(ID)
  , config()
  , F(nullptr)
  , PassSE(nullptr)
  , enableDiagOutput(false)
  , introduced(false)
  , vectorizer()
  {}

  bool runOnFunction(llvm::Function &F) override;

  /// Register all analyses and transformation required.
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

private:
  Config config;
  llvm::Function * F;
  llvm::ScalarEvolution * PassSE;

  bool enableDiagOutput;
  bool introduced;

  // cost estimation
  struct LoopScore {
    LoopScore() : Score(0) {}
    unsigned Score;
  };
  
  struct LoopJob {
    LoopJob()
    : Header(nullptr)
    , VectorWidth(0)
    , DepDist(0)
    , TripAlign(0)
    {}

    llvm::BasicBlock *Header;
    unsigned VectorWidth;

    iter_t DepDist; // minimal dependence distance between loop iterations
    iter_t TripAlign; // multiple of loop trip count
  };

  /// \return true if legal (in that case LJ&LS get populated)
  bool scoreLoop(LoopJob& LJ, LoopScore& LS, llvm::Loop & L);

  // Step 1: Decide which loops to vectorize.
  // Step 2: Prepare all loops for vectorization.
  // Step 3: Vectorize the regions.
  bool collectLoopJobs(llvm::LoopInfo & LI);
  std::vector<LoopJob> LoopsToPrepare;
  bool prepareLoopVectorization(); // TODO mark this function as un-vectorizable (or prepare a holdout copy)

  struct LoopVectorizerJob {
    LoopJob LJ;
    ValueSet uniOverrides;
    llvm::Value *EntryAVL; // loop entry AVL (FIXME)
  };
  std::vector<LoopVectorizerJob> LoopsToVectorize;
  bool vectorizeLoopRegions();
  bool vectorizeLoop(LoopVectorizerJob& LVJob);

  llvm::FunctionAnalysisManager FAM; // private pass infrastructure
  std::unique_ptr<VectorizerInterface> vectorizer;

  bool canVectorizeLoop(llvm::Loop &L);
  bool hasVectorizableLoopStructure(llvm::Loop &L);

  // convert L into a vectorizable loop
  // this will create a new scalar loop that can be vectorized directly with RV
  PreparedLoop transformToVectorizableLoop(llvm::Loop &L, int VectorWidth, int tripAlign, ValueSet & uniformOverrides);

  bool canAdjustTripCount(llvm::Loop &L, int VectorWidth, int TripCount);

  // return the trip count of L if it is constant. Otw, returns -1
  int getTripCount(llvm::Loop &L);

  // the trip count of the loop is always a multiple of this value
  // returns 1 for loop w/o known alignment
  int getTripAlignment(llvm::Loop & L);

  // vectorize all loops
  bool runLoopJobs();
};

struct LoopVectorizerWrapperPass : llvm::PassInfoMixin<LoopVectorizerWrapperPass> {
  private:
    std::shared_ptr<llvm::FunctionPass> loopvec;
  public:
    LoopVectorizerWrapperPass() : loopvec(rv::createLoopVectorizerPass()) {};

    llvm::PreservedAnalyses run (llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {
      if (loopvec->runOnFunction(F))
        return llvm::PreservedAnalyses::none();
      else
        return llvm::PreservedAnalyses::all();
    }
};

} // namespace rv
