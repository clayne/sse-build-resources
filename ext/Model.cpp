#include "Model.h"

#include <skse64/GameStreams.h>
#include <skse64/NiRTTI.h>
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
				stl::snprintf(a_buffer, "%s\\%s", a_prefix, a_path);
				a_out = a_buffer;
			}

			return true;
		}

		bool ModelLoader::LoadObject(
			const char* a_path,
			NiPointer<NiNode>& a_out)
		{
			BSResourceNiBinaryStream binaryStream(a_path);
			if (!binaryStream.IsValid())
			{
				return false;
			}

			auto& stream = GetStream();

			if (!stream->LoadStream(std::addressof(binaryStream)))
			{
				return false;
			}

			if (!stream->m_rootObjects.m_data)
			{
				return false;
			}

			for (auto& e : stream->m_rootObjects)
			{
				if (!e)
				{
					continue;
				}

				if (auto object = ni_cast(e, NiNode))
				{
					BSShaderResourceManager::GetSingleton()->ConvertLegacy(object, false);

					a_out.reset(object);

					return true;
				}
				else
				{
					return ConstructObject(stream, a_out);
				}
			}

			return false;
		}

		bool ModelLoader::ConstructObject(
			const Stream::NiStreamWrapper& a_stream,
			NiPointer<NiNode>& a_out)
		{
			a_out.reset();

			for (auto& e : a_stream->m_rootObjects)
			{
				if (!e)
				{
					continue;
				}

				if (auto object = ni_cast(e, NiAVObject))
				{
					if (!a_out)
					{
						a_out = NiNode::Create(1);
						a_out->m_flags = NiAVObject::kFlag_SelectiveUpdate |
						                 NiAVObject::kFlag_SelectiveUpdateTransforms |
						                 NiAVObject::kFlag_kSelectiveUpdateController;
					}

					BSShaderResourceManager::GetSingleton()->ConvertLegacy(object, false);

					a_out->AttachChild(object, true);
				}
			}

			return a_out != nullptr;
		}

	}
}