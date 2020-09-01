#pragma once

#define _assert(a) { if(!(a)) { gLogger.FatalError("Assertion failed in %s (%d): %s", __FILE__, __LINE__, #a); abort(); } }
