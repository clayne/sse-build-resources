#pragma once

#include "Stream.h"

#include <skse64/GameReferences.h>
#include <skse64/NiNodes.h>

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
            bool LoadObject(
                const char* a_model,
                NiPointer<NiNode>& a_out);

            SKMP_FORCEINLINE auto &GetStream() const {
                return m_stream;
            }

        private:

            Stream::NiStreamWrapper m_stream;
        };
    }
}