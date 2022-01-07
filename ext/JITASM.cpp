#include "JITASM.h"

#include <skse64_common/BranchTrampoline.h>

namespace JITASM
{
	JITASM::JITASM(
		BranchTrampoline& a_trampoline,
		std::size_t a_maxSize) :
		_m_trampoline(a_trampoline),
		Xbyak::CodeGenerator(
			a_maxSize,
			a_trampoline.StartAlloc())
	{
	}

	void JITASM::done()
	{
		if (!_endedAlloc)
		{
			_endedAlloc = true;
			_m_trampoline.EndAlloc(static_cast<const void*>(getCurr()));
		}
	}
}