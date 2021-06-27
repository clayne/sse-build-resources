#pragma once

#include "xbyak/xbyak.h"

namespace JITASM
{
    class JITASM
        : public Xbyak::CodeGenerator
    {
    public:
        JITASM(BranchTrampoline& a_trampoline, std::size_t maxSize = Xbyak::DEFAULT_MAX_CODE_SIZE);

        void done();
        std::uintptr_t get();

        template <class T>
        T get()
        {
            done();
            return reinterpret_cast<T>(getCode());
        }

    private:

        bool _endedAlloc;
        BranchTrampoline& _m_trampoline;
    };
}