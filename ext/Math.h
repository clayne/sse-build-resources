#pragma once

#include <algorithm>

namespace Math
{
    SKMP_FORCEINLINE float Normalize(float a_val, float a_min, float a_max) {
        return (a_val - a_min) / (a_max - a_min);
    }

    SKMP_FORCEINLINE float NormalizeClamp(float a_val, float a_min, float a_max) {
        return std::clamp(Normalize(a_val, a_min, a_max), 0.0f, 1.0f);
    }
}
