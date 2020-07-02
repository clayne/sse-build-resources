#include "JITASM.h"

#include "skse64_common/BranchTrampoline.h"

JITASM::JITASM(size_t maxSize) :
	_endedAlloc(false),
	Xbyak::CodeGenerator(maxSize, g_localTrampoline.StartAlloc())
{}

void JITASM::done()
{
	if (!_endedAlloc) {
		_endedAlloc = true;
		g_localTrampoline.EndAlloc(getCurr());
	}
}

uintptr_t JITASM::get()
{
	done();
	return reinterpret_cast<uintptr_t>(getCode());
}
