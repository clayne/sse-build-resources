#pragma once

#include "skse64/PluginAPI.h"
#include "skse64_common/BranchTrampoline.h"

namespace Hook
{

#pragma pack(push, 1)
    struct CB5Code
    {
        uint8_t	op;
        int32_t	displ;
    };

    struct CB6Code
    {
        uint8_t	escape;
        uint8_t	modrm;
        uint32_t displ;
    };
#pragma pack(pop)

    static_assert(sizeof(CB5Code) == 0x5);
    static_assert(sizeof(CB6Code) == 0x6);

    template <uint8_t op>
    bool CheckDst5(uintptr_t addr)
    {
        static_assert(op == 0xE8 || op == 0xE9, "invalid opcode");

        auto ins = reinterpret_cast<CB5Code*>(addr);

        if (ins->op != op) {
            return false;
        }

        return true;
    }

    template <uint8_t op>
    bool GetDst5(uintptr_t addr, uintptr_t& out)
    {
        static_assert(op == 0xE8 || op == 0xE9, "invalid opcode");

        auto ins = reinterpret_cast<CB5Code*>(addr);

        if (ins->op != op) {
            return false;
        }

        out = addr + sizeof(CB5Code) + ins->displ;

        return true;
    }

    template <uint8_t modrm>
    bool GetDst6(uintptr_t addr, uintptr_t& out)
    {
        static_assert(modrm == 0x15 || modrm == 0x25, "invalid modr/m byte");

        auto ins = reinterpret_cast<CB6Code*>(addr);

        if (ins->escape != 0xFF || ins->modrm != modrm) {
            return false;
        }

        out = *reinterpret_cast<uintptr_t*>(addr + sizeof(CB6Code) + ins->displ);

        return true;
    }

    template <typename T>
    bool Call5(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst5<0xE8>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write5Call(addr, dst);

        return true;
    }

    template <typename T>
    bool Jmp5(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst5<0xE9>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write5Branch(addr, dst);

        return true;
    }

    template <typename T>
    bool Call6(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst6<0x15>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write6Call(addr, dst);

        return true;
    }

    template <typename T>
    bool Jmp6(uintptr_t addr, uintptr_t dst, T& orig)
    {
        uintptr_t o;
        if (!GetDst6<0x25>(addr, o)) {
            return false;
        }

        orig = reinterpret_cast<T>(o);

        g_branchTrampoline.Write6Branch(addr, dst);

        return true;
    }

    size_t GetAllocGranularity();
    size_t GetAlignedTrampolineSize(size_t maxSize);

    size_t InitBranchTrampoline(const SKSEInterface* skse, size_t a_size);
    size_t InitLocalTrampoline(const SKSEInterface* skse, size_t a_size);
}