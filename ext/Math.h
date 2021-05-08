#pragma once

#include <algorithm>

namespace Math
{
    SKMP_FORCEINLINE float Normalize(float a_val, float a_min, float a_max) {
        return (a_val - a_min) / (a_max - a_min);
    }

    SKMP_FORCEINLINE float NormalizeSafe(float a_val, float a_min, float a_max) {
        float s = (a_max - a_min);
        if (s == 0.0f) {
            return 0.0f;
        }
        return (a_val - a_min) / s;
    }

    SKMP_FORCEINLINE float NormalizeClamp(float a_val, float a_min, float a_max) {
        return std::clamp(Normalize(a_val, a_min, a_max), 0.0f, 1.0f);
    }

    SKMP_FORCEINLINE float NormalizeSafeClamp(float a_val, float a_min, float a_max) {
        return std::clamp(NormalizeSafe(a_val, a_min, a_max), 0.0f, 1.0f);
    }
}
