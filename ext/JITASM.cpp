#include "JITASM.h"

#include <skse64_common/BranchTrampoline.h>

namespace JITASM
{
	JITASM::JITASM(BranchTrampoline &a_trampoline, std::size_t maxSize) :
		_endedAlloc(false),
		_m_trampoline(a_trampoline),
		Xbyak::CodeGenerator(maxSize, a_trampoline.StartAlloc())
	{}

	void JITASM::done()
	{
		if (!_endedAlloc) {
			_endedAlloc = true;
			_m_trampoline.EndAlloc(getCurr());
		}
	}

	std::uintptr_t JITASM::get()
	{
		done();
		return reinterpret_cast<std::uintptr_t>(getCode());
	}
}