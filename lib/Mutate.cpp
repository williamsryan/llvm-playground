#include "Mutate.h"

#include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "mutate"

//-----------------------------------------------------------------------------
// Mutate Implementation.
//-----------------------------------------------------------------------------
bool Mutate::runOnBasicBlock(BasicBlock &BB)
{
    bool Changed = false;

    for (auto Inst = BB.begin(), IE = BB.end(); Inst != IE; ++Inst)
    {
        // Skip non-binary (e.g. unary or compare) instruction.
        auto *BinOp = dyn_cast<llvm::StoreInst>(Inst);
        if (!BinOp)
            continue;

        errs() << "[+] Test: " << *BinOp << "\n";

        // Skip instructions other than integer sub.
        unsigned Opcode = BinOp->getOpcode();
        StringRef Name = Inst->getOpcodeName();
        errs() << "[+] Opcode Name:\t" << Name << "\n";

        // Skip instructions other than add.
        // if (BinOp->getOpcode() != Instruction::Store)
        //     continue;

        IRBuilder<> Builder(BinOp);

        // Target the instruction that we want our NOP to be placed before/after.
        Instruction *I = BB.getFirstNonPHIOrDbg();
        *I->getNextNode();

        assert(I);
        // assert(terminator);

        ConstantInt *nonce = llvm::ConstantInt::get(BB.getContext(), llvm::APInt(32, StringRef("1337"), 10));
        // auto *NewInst = new llvm::StoreInst(nonce, BinOp);
        // StoreInst *strInst = Builder.CreateStore(nonce, BinOp->getNextNode());
        AllocaInst *allocaInst = Builder.CreateAlloca(Type::getInt32Ty(BB.getContext()), 0, "myIntVar");
        // ReplaceInstWithInst(BB.getInstList(), Inst, strInst);
        // BB.getInstList().insert(Inst, strInst);

        Builder.CreateStore(nonce, allocaInst);

        Changed = true;
    }
    return Changed;
}

PreservedAnalyses Mutate::run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &)
{
    bool Changed = false;

    for (auto &BB : F)
    {
        // errs() << "BB:\t" << BB << "\n";
        Changed |= runOnBasicBlock(BB);
    }
    return (Changed ? llvm::PreservedAnalyses::none()
                    : llvm::PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMutatePluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "mutate", LLVM_VERSION_STRING,
            [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "mutate")
                        {
                            FPM.addPass(Mutate());
                            return true;
                        }
                        return false;
                    });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return getMutatePluginInfo();
}
