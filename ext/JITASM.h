#pragma once

#include "xbyak/xbyak.h"

class JITASM
	: public Xbyak::CodeGenerator
{
public:
	JITASM(size_t maxSize = Xbyak::DEFAULT_MAX_CODE_SIZE);

	void done();
	uintptr_t get();

private:
	bool _endedAlloc;
};
