#ifndef MUTATE_H
#define MUTATE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

// PassInfoMixIn is a CRTP mix-in to automatically provide informational APIs
// needed for passes. Currently it provides only the 'name' method.
struct Mutate : public llvm::PassInfoMixin<Mutate>
{
    llvm::PreservedAnalyses run(llvm::Function &F,
                                llvm::FunctionAnalysisManager &);

    static bool runOnBasicBlock(llvm::BasicBlock &B);
};

struct LegacyMutate : public llvm::FunctionPass
{
    // The address of this static is used to uniquely identify this pass in the
    // pass registry. The PassManager relies on this address to find instance of
    // analyses passes and build dependencies on demand.
    // The value does not matter.
    static char ID;

    LegacyMutate() : FunctionPass(ID) {}

    bool runOnFunction(llvm::Function &F) override;

    Mutate Impl;
};

#endif // MUTATE_H