//
// Created by Ryosuke Iwanaga on 2018-11-04.
//

#ifndef LLVM_FORTH_STACK_H
#define LLVM_FORTH_STACK_H

#include "core.h"
#include "dict.h"
#include "util.h"

namespace stack {
    static Value* SP;
    static Constant* Stack;
    static Value* RSP;
    static Constant* RStack;

    static void Push(Value* value) {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), current_sp});
        core::Builder.CreateStore(value, addr);
        core::Builder.CreateStore(core::Builder.CreateAdd(current_sp, core::GetIndex(1)), SP);
    }
    
    static void PushPtr(Value* value) {
        Push(core::Builder.CreatePtrToInt(value, core::IntType));
    }

    static LoadInst* Peek() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto top_sp = core::Builder.CreateSub(current_sp, core::GetIndex(1));
        auto addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), top_sp});
        return core::Builder.CreateLoad(addr);
    }

    static LoadInst* Pop() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto top_sp = core::Builder.CreateSub(current_sp, core::GetIndex(1));
        auto addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), top_sp});
        core::Builder.CreateStore(top_sp, SP);
        return core::Builder.CreateLoad(addr);
    }
    
    static Value* PopPtr(Type* ptr_type) {
        return core::Builder.CreateIntToPtr(Pop(), ptr_type);
    }

    static void Drop() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto top_sp = core::Builder.CreateSub(current_sp, core::GetIndex(1));
        auto addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), top_sp});
        core::Builder.CreateStore(top_sp, SP);
    }

    static void Dup() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto current_addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), current_sp});
        auto top_sp = core::Builder.CreateSub(current_sp, core::GetIndex(1));
        auto top_addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), top_sp});
        core::Builder.CreateStore(core::Builder.CreateLoad(top_addr), current_addr);
        core::Builder.CreateStore(core::Builder.CreateAdd(current_sp, core::GetIndex(1)), SP);
    }

    static void Over() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto current_addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), current_sp});
        auto second_sp = core::Builder.CreateSub(current_sp, core::GetIndex(2));
        auto second_addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), second_sp});
        core::Builder.CreateStore(core::Builder.CreateLoad(second_addr), current_addr);
        core::Builder.CreateStore(core::Builder.CreateAdd(current_sp, core::GetIndex(1)), SP);
    }

    static void Print() {
        auto current_sp = core::Builder.CreateLoad(SP);
        auto top_sp = core::Builder.CreateSub(current_sp, core::GetIndex(1));
        auto start_addr = core::Builder.CreateGEP(Stack, {core::GetIndex(0), core::GetIndex(0)});
        core::CallFunction(util::PrintStackFunc, {top_sp, start_addr});
    }

    static void RPush(Value* value) {
        auto current_rsp = core::Builder.CreateLoad(RSP);
        auto addr = core::Builder.CreateGEP(RStack, {core::GetIndex(0), current_rsp});
        core::Builder.CreateStore(value, addr);
        core::Builder.CreateStore(core::Builder.CreateAdd(current_rsp, core::GetIndex(1)), RSP);
    }

    static LoadInst* RPop() {
        auto current_rsp = core::Builder.CreateLoad(RSP);
        auto top_rsp = core::Builder.CreateSub(current_rsp, core::GetIndex(1));
        auto addr = core::Builder.CreateGEP(RStack, {core::GetIndex(0), top_rsp});
        core::Builder.CreateStore(top_rsp, RSP);
        return core::Builder.CreateLoad(addr);
    }

    static void RDup() {
        auto current_rsp = core::Builder.CreateLoad(RSP);
        auto current_addr = core::Builder.CreateGEP(RStack, {core::GetIndex(0), current_rsp});
        auto top_rsp = core::Builder.CreateSub(current_rsp, core::GetIndex(1));
        auto top_addr = core::Builder.CreateGEP(RStack, {core::GetIndex(0), top_rsp});
        core::Builder.CreateStore(core::Builder.CreateLoad(top_addr), current_addr);
        core::Builder.CreateStore(core::Builder.CreateAdd(current_rsp, core::GetIndex(1)), RSP);
    }

    static Value* RPick(Value* n) {
        auto current_rsp = core::Builder.CreateLoad(RSP);
        auto offset = core::Builder.CreateAdd(n, core::GetInt(1));
        auto pick_rsp = core::Builder.CreateSub(current_rsp, core::Builder.CreateIntCast(offset, core::IndexType, true));
        auto addr = core::Builder.CreateGEP(RStack, {core::GetIndex(0), pick_rsp});
        return core::Builder.CreateLoad(addr);
    }

    static void Initialize(Function* main, BasicBlock* entry) {
        SP = core::Builder.CreateAlloca(core::IndexType, nullptr, "sp");
        core::Builder.CreateStore(core::GetIndex(0), SP);
        Stack = core::CreateGlobalArrayVariable("stack", core::IntType, 1024, false);
        
        RSP = core::Builder.CreateAlloca(core::IndexType, nullptr, "rsp");
        core::Builder.CreateStore(core::GetIndex(0), RSP);
        RStack = core::CreateGlobalArrayVariable("rstack", dict::XtPtrPtrType, 1024, false);
    }
}

#endif //LLVM_FORTH_STACK_H
