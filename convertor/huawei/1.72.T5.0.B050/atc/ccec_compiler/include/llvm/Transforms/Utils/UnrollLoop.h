//===- llvm/Transforms/Utils/UnrollLoop.h - Unrolling utilities -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines some loop unrolling utilities. It does not define any
// actual pass or policy, but provides a single function to perform loop
// unrolling.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_UNROLLLOOP_H
#define LLVM_TRANSFORMS_UTILS_UNROLLLOOP_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

namespace llvm {

class AssumptionCache;
class BasicBlock;
class DependenceInfo;
class DominatorTree;
class Loop;
class LoopInfo;
class MDNode;
class OptimizationRemarkEmitter;
class ScalarEvolution;

using NewLoopsMap = SmallDenseMap<const Loop *, Loop *, 4>;
using InstrPairListTy = std::vector<std::pair<CmpInst *, Instruction *>>;

/// @{
/// Metadata attribute names
const char *const LLVMLoopUnrollFollowupAll = "llvm.loop.unroll.followup_all";
const char *const LLVMLoopUnrollFollowupUnrolled =
    "llvm.loop.unroll.followup_unrolled";
const char *const LLVMLoopUnrollFollowupRemainder =
    "llvm.loop.unroll.followup_remainder";
/// @}

struct LoopBounds {
  Value *lowerBound;
  Value *upperBound;
};

const Loop* addClonedBlockToLoopInfo(BasicBlock *OriginalBB,
                                     BasicBlock *ClonedBB, LoopInfo *LI,
                                     NewLoopsMap &NewLoops);

/// Represents the result of a \c UnrollLoop invocation.
enum class LoopUnrollResult {
  /// The loop was not modified.
  Unmodified,

  /// The loop was partially unrolled -- we still have a loop, but with a
  /// smaller trip count.  We may also have emitted epilogue loop if the loop
  /// had a non-constant trip count.
  PartiallyUnrolled,

  /// The loop was fully unrolled into straight-line code.  We no longer have
  /// any back-edges.
  FullyUnrolled
};

// need to fix the caller for parameters
LoopUnrollResult
UnrollLoop(Loop *L, unsigned Count, unsigned TripCount, bool Force,
           bool AllowRuntime, bool AllowExpensiveTripCount, bool PreserveCondBr,
           bool PreserveOnlyFirst, unsigned TripMultiple, unsigned PeelCount,
           bool UnrollRemainder, LoopInfo *LI, ScalarEvolution *SE,
           DominatorTree *DT, AssumptionCache *AC,
           OptimizationRemarkEmitter *ORE, bool PreserveLCSSA,
           InstrPairListTy &PeelPattern, Loop **RemainderLoop = nullptr,
           LoopBounds *LoopBounds = nullptr, unsigned PeelLastCount = 0);

bool UnrollRuntimeLoopRemainder(Loop *L, unsigned Count,
                                bool AllowExpensiveTripCount,
                                bool UseEpilogRemainder, bool UnrollRemainder,
                                LoopInfo *LI, ScalarEvolution *SE,
                                DominatorTree *DT, AssumptionCache *AC,
                                bool PreserveLCSSA,
                                Loop **ResultLoop = nullptr);

// need to fix the caller for parameters
void computePeelCount(Loop *L, unsigned LoopSize,
                      TargetTransformInfo::UnrollingPreferences &UP,
                      const unsigned &TripCount, ScalarEvolution &SE,
                      const TargetTransformInfo &TTI,
                      InstrPairListTy &PeelPattern, LoopBounds *LoopBounds,
                      bool PreserveLCSSA);

bool canPeel(Loop *L);

bool peelLoop(Loop *L, unsigned PeelCount, LoopInfo *LI, ScalarEvolution *SE,
              DominatorTree *DT, AssumptionCache *AC, bool PreserveLCSSA,
              LoopBounds *loopBounds, bool reversePeel,
              InstrPairListTy &PeelPattern);

LoopUnrollResult UnrollAndJamLoop(Loop *L, unsigned Count, unsigned TripCount,
                                  unsigned TripMultiple, bool UnrollRemainder,
                                  LoopInfo *LI, ScalarEvolution *SE,
                                  DominatorTree *DT, AssumptionCache *AC,
                                  OptimizationRemarkEmitter *ORE,
                                  Loop **EpilogueLoop = nullptr);

bool isSafeToUnrollAndJam(Loop *L, ScalarEvolution &SE, DominatorTree &DT,
                          DependenceInfo &DI);

bool computeUnrollCount(Loop *L, const TargetTransformInfo &TTI,
                        DominatorTree &DT, LoopInfo *LI, ScalarEvolution &SE,
                        const SmallPtrSetImpl<const Value *> &EphValues,
                        OptimizationRemarkEmitter *ORE, unsigned &TripCount,
                        unsigned MaxTripCount, unsigned &TripMultiple,
                        unsigned LoopSize,
                        TargetTransformInfo::UnrollingPreferences &UP,
                        bool &UseUpperBound, InstrPairListTy &PeelPattern,
                        AssumptionCache *AC = nullptr,
                        LoopBounds *loopBounds = nullptr);
bool calculatePatternList(Loop *L, InstrPairListTy &PeelPattern,
                          ScalarEvolution &SE, LoopBounds *loopBounds);
bool calculateLoopBounds(Loop *L, ScalarEvolution &SE, LoopBounds *loopBounds);

BasicBlock *foldBlockIntoPredecessor(BasicBlock *BB, LoopInfo *LI,
                                     ScalarEvolution *SE, DominatorTree *DT);

bool findIndVarBranches(Loop *L, InstrPairListTy &PeelPattern,
                        ScalarEvolution &SE, LoopBounds *loopBounds);

void remapInstruction(Instruction *I, ValueToValueMapTy &VMap);

void simplifyLoopAfterUnroll(Loop *L, bool SimplifyIVs, LoopInfo *LI,
                             ScalarEvolution *SE, DominatorTree *DT,
                             AssumptionCache *AC);

bool calculateLoopBounds(Loop *L, ScalarEvolution &SE, LoopBounds *loopBounds);

bool isFirstIterationCondition(CmpInst *cmp, LoopBounds *loopBounds);

bool isLastIterationCondition(CmpInst *cmpI, LoopBounds *loopBounds);

void cleanUpAfterPeeling(Loop *L, DominatorTree &DT, LoopInfo *LI,
                         ScalarEvolution *SE, AssumptionCache &AC,
                         InstrPairListTy &PeelPattern, PHINode *ivPH,
                         LoopBounds *loopBounds);

void fixUpLoopCFG(Loop *L, DominatorTree *DT, LoopInfo *LI, ScalarEvolution *SE,
                  AssumptionCache *AC);

MDNode *GetUnrollMetadata(MDNode *LoopID, StringRef Name);

TargetTransformInfo::UnrollingPreferences gatherUnrollingPreferences(
    Loop *L, ScalarEvolution &SE, const TargetTransformInfo &TTI, int OptLevel,
    Optional<unsigned> UserThreshold, Optional<unsigned> UserCount,
    Optional<bool> UserAllowPartial, Optional<bool> UserRuntime,
    Optional<bool> UserUpperBound, Optional<bool> UserAllowPeeling);

unsigned ApproximateLoopSize(const Loop *L, unsigned &NumCalls,
                             bool &NotDuplicatable, bool &Convergent,
                             const TargetTransformInfo &TTI,
                             const SmallPtrSetImpl<const Value *> &EphValues,
                             unsigned BEInsns);

} // end namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_UNROLLLOOP_H
