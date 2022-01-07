#pragma once

#include "xbyak/xbyak.h"

namespace JITASM
{
	class JITASM : public Xbyak::CodeGenerator
	{
	public:
		JITASM(
			BranchTrampoline& a_trampoline,
			std::size_t a_maxSize = Xbyak::DEFAULT_MAX_CODE_SIZE);

		void done();

		template <
			class T = std::uintptr_t,
			class Tr = std::conditional_t<
				std::is_pointer_v<T>,
				const std::remove_pointer_t<T>*,
				T>>
		Tr get()
		{
			done();

			if constexpr (std::is_same_v<std::remove_const_t<Tr>, std::uint8_t*>)
			{
				return getCode();
			}
			else
			{
				return reinterpret_cast<Tr>(getCode());
			}
		}

	private:
		bool _endedAlloc{ false };
		BranchTrampoline& _m_trampoline;
	};
}