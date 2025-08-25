#include <llvm/LinkAllIR.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Debug.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include <iostream>
#include <fstream>
#include <dlfcn.h>

using namespace std;

string load2str(const string& sFullFileName) {
    ifstream ifs(sFullFileName.c_str());

    return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}

struct LLVM_ObjectCache : public llvm::ObjectCache {
    LLVM_ObjectCache(string const& buf) {
        buf_ = llvm::MemoryBuffer::getMemBuffer(buf);
    }

    void notifyObjectCompiled(const llvm::Module* M,
                              llvm::MemoryBufferRef Obj) override {}

    std::unique_ptr<llvm::MemoryBuffer> getObject(
        const llvm::Module* M) override {
        return std::move(buf_);
    }

    std::unique_ptr<llvm::MemoryBuffer> buf_;
};

void init() {
    llvm::DebugFlag = true;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
}

void loadDylib() {
    bool ok = ::dlopen("dylib/libdylib.so", RTLD_NOW | RTLD_GLOBAL);
    if (!ok) {
        cout << "dlopen error. please run ./build.sh in dylib" << endl;
        exit(1);
    }
}

extern "C" {
int pow2(int val) { return val * val; }
}

int main(int argc, char** argv) {
    init();
    loadDylib();

    llvm::LLVMContext context;
    string errMsg;

    std::unique_ptr<llvm::Module> module =
        llvm::make_unique<llvm::Module>("MyModule", context);

    llvm::ExecutionEngine* ee =
        llvm::EngineBuilder(move(module))
            .setEngineKind(llvm::EngineKind::JIT)
            .setMCJITMemoryManager(std::unique_ptr<llvm::RTDyldMemoryManager>(
                new llvm::SectionMemoryManager))
            .setErrorStr(&errMsg)
            .setVerifyModules(true)
            .create();

    if (!errMsg.empty()) {
        cout << "create error:" << errMsg << endl;
        exit(1);
    }

    ee->addGlobalMapping("pow2", (uint64_t)&pow2);

    string fileName = argv[1];

    string fileStr = load2str(fileName);
    if (fileStr.empty()) {
        cout << "load2Str error:" << fileStr << endl;
        exit(1);
    }

    LLVM_ObjectCache objCache(fileStr);
    ee->setObjectCache(&objCache);
    ee->finalizeObject();
    ee->setObjectCache(nullptr);

    uint64_t addr = ee->getFunctionAddress("pow4");

    typedef int (*pow4_t)(int);
    pow4_t fn = (pow4_t)addr;
    auto result = fn(2);
    cout << result << endl;
    return 0;
}