
#include "Model.h"

#include <skse64/GameStreams.h>
#include <skse64/NiRenderer.h>

namespace Util
{
	namespace Model
	{
		bool MakePath(
			const char* a_prefix,
			const char* a_path,
			char (&a_buffer)[MAX_PATH],
			const char*& a_out)
		{
			if (!a_path)
			{
				return false;
			}

			auto prefixLen = std::strlen(a_prefix);
			auto pathLen = std::strlen(a_path);

			if (_strnicmp(a_prefix, a_path, std::min(prefixLen, pathLen)) == 0)
			{
				a_out = a_path;
			}
			else
			{
				_snprintf_s(a_buffer, _TRUNCATE, "%s\\%s", a_prefix, a_path);
				a_out = a_buffer;
			}

			return true;
		}

		bool ModelLoader::Load(
			const char* a_path)
		{
			using namespace Stream;

			BSResourceNiBinaryStream binaryStream(a_path);
			if (!binaryStream.IsValid())
			{
				return false;
			}

			if (!m_stream->LoadStream(std::addressof(binaryStream))) {
				return false;
			}

			return m_stream->m_rootObjects.m_data != nullptr;
		}

		bool ModelLoader::LoadObject(
			const char* a_model,
			NiPointer<NiNode>& a_out)
		{
			if (!Load(a_model))
			{
				return false;
			}

			auto& stream = GetStream();

			/*std::uint64_t tot = 0;

			for (auto& e : stream->m_objectSizes)
			{
				tot += e;
			}

			_DMESSAGE("%s: %llu", a_model, tot); */

			for (auto e : stream->m_rootObjects)
			{
				if (!e)
				{
					continue;
				}

				if (auto object = ni_cast(e, NiNode))
				{
					(*g_shaderResourceManager)->ConvertLegacy(object, false);

					a_out.reset(object);

					return true;
				}
			}

			return false;
		}

	}
}