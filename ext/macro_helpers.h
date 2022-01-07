#pragma once

#define _MK_STRING(x) #x
#define MK_STRING(x) _MK_STRING(x)

#define MK_GIT_COMMIT(x) 0x##x
