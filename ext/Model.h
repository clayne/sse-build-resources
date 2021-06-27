#pragma once

#include "Stream.h"

#include <skse64/GameReferences.h>

namespace Util
{
    namespace Model
    {
        bool GetRootModelPath(
            TESObjectREFR* a_refr,
            bool a_firstPerson,
            bool a_isFemale,
            const char*& a_out);

        class ModelLoader
        {
        public:

            ModelLoader() = default;

            bool Load(const char* a_path);

            SKMP_FORCEINLINE auto &GetStream() const {
                return m_stream;
            }

        private:

            Stream::NiStreamWrapper m_stream;
        };
    }
}