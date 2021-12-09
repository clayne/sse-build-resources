#pragma once

#include "Stream.h"

#include <skse64/GameReferences.h>
#include <skse64/NiNodes.h>

namespace Util
{
	namespace Model
	{
		bool MakePath(
			const char* a_prefix,
			const char* a_path,
			char (&a_buffer)[MAX_PATH],
			const char*& a_out);

		class ModelLoader
		{
		public:
			ModelLoader() = default;

			bool Load(const char* a_path);
			bool LoadObject(
				const char* a_model,
				NiPointer<NiNode>& a_out);

			inline constexpr const auto& GetStream() const noexcept
			{
				return m_stream;
			}

		private:
			Stream::NiStreamWrapper m_stream;
		};
	}
}