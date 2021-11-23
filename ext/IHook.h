#pragma once

#include "skse64/PluginAPI.h"
#include "skse64_common/BranchTrampoline.h"

namespace Hook
{
#pragma pack(push, 1)
	struct CB5Code
	{
		std::uint8_t op;
		std::int32_t displ;
	};

	struct CB6Code
	{
		std::uint8_t escape;
		std::uint8_t modrm;
		std::int32_t displ;
	};
#pragma pack(pop)

	template <std::uint8_t op>
	bool CheckDst5(std::uintptr_t addr)
	{
		static_assert(op == std::uint8_t(0xE8) || op == std::uint8_t(0xE9), "invalid opcode");

		auto ins = reinterpret_cast<CB5Code*>(addr);

		return ins->op == op;
	}

	template <std::uint8_t op, class T>
	bool GetDst5(std::uintptr_t addr, T& out)
	{
		static_assert(op == std::uint8_t(0xE8) || op == std::uint8_t(0xE9), "invalid opcode");

		auto ins = reinterpret_cast<CB5Code*>(addr);

		if (ins->op != op)
		{
			return false;
		}

		auto oa = addr + sizeof(CB5Code) + ins->displ;

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			out = oa;
		}
		else
		{
			out = reinterpret_cast<T>(oa);
		}

		return true;
	}

	template <std::uint8_t modrm, class T>
	bool GetDst6(std::uintptr_t addr, T& out)
	{
		static_assert(modrm == std::uint8_t(0x15) || modrm == std::uint8_t(0x25), "invalid modr/m byte");

		auto ins = reinterpret_cast<CB6Code*>(addr);

		if (ins->escape != 0xFF || ins->modrm != modrm)
		{
			return false;
		}

		auto oa = *reinterpret_cast<std::uintptr_t*>(addr + sizeof(CB6Code) + ins->displ);

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			out = oa;
		}
		else
		{
			out = reinterpret_cast<T>(oa);
		}

		return true;
	}

	template <typename T>
	bool Call5(BranchTrampoline& a_trampoline, std::uintptr_t addr, std::uintptr_t dst, T& orig)
	{
		std::uintptr_t o;

		if (!GetDst5<0xE8>(addr, o))
		{
			return false;
		}

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			orig = o;
		}
		else
		{
			orig = reinterpret_cast<T>(o);
		}

		a_trampoline.Write5Call(addr, dst);

		return true;
	}

	template <typename T>
	bool Jmp5(BranchTrampoline& a_trampoline, std::uintptr_t addr, std::uintptr_t dst, T& orig)
	{
		std::uintptr_t o;

		if (!GetDst5<0xE9>(addr, o))
		{
			return false;
		}

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			orig = o;
		}
		else
		{
			orig = reinterpret_cast<T>(o);
		}

		a_trampoline.Write5Branch(addr, dst);

		return true;
	}

	template <typename T>
	bool Call6(BranchTrampoline& a_trampoline, std::uintptr_t addr, std::uintptr_t dst, T& orig)
	{
		std::uintptr_t o;

		if (!GetDst6<0x15>(addr, o))
		{
			return false;
		}

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			orig = o;
		}
		else
		{
			orig = reinterpret_cast<T>(o);
		}

		a_trampoline.Write6Call(addr, dst);

		return true;
	}

	template <typename T>
	bool Jmp6(BranchTrampoline& a_trampoline, std::uintptr_t addr, std::uintptr_t dst, T& orig)
	{
		std::uintptr_t o;

		if (!GetDst6<0x25>(addr, o))
		{
			return false;
		}

		if constexpr (std::is_same_v<T, std::uintptr_t>)
		{
			orig = o;
		}
		else
		{
			orig = reinterpret_cast<T>(o);
		}

		a_trampoline.Write6Branch(addr, dst);

		return true;
	}
}