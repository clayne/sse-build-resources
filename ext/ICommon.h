#pragma once

#define FN_NAMEPROC(x) virtual const char *ModuleName() { return x; };

#include "IMisc.h"
#include "PerfCounter.h"
#include "AddressLibrary.h"
#include "RTTI.h"
#include "Patching.h"
#include "IHook.h"
#include "ILogging.h"
#include "IAssert.h"
