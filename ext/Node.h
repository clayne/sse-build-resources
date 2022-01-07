#pragma once

#include <skse64/GameReferences.h>
#include <skse64/GameTypes.h>
#include <skse64/NiNodes.h>

namespace Util
{
	namespace Node
	{
		[[nodiscard]] NiNode* FindNode(
			NiNode* a_root,
			const BSFixedString& a_name);

		[[nodiscard]] NiAVObject* FindChild(NiNode* a_node, const BSFixedString& a_name);
		[[nodiscard]] NiNode* FindChildNode(NiNode* a_node, const BSFixedString& a_name);

		class NiRootNodes
		{
		public:
			NiRootNodes(TESObjectREFR* const a_ref, bool a_no1p = false);

			[[nodiscard]] bool MatchesAny(NiNode* const a_node);

			void GetNPCRoots(const BSFixedString& a_npcroot);

			NiPointer<NiNode> m_nodes[2];
		};

		enum class VisitorControl
		{
			kContinue,
			kStop
		};

		template <class Tp>
		VisitorControl Traverse(NiAVObject* a_object, Tp a_func)
		{
			if (!a_object)
			{
				return VisitorControl::kContinue;
			}

			if (a_func(a_object) == VisitorControl::kStop)
			{
				return VisitorControl::kStop;
			}

			auto node = a_object->GetAsNiNode();
			if (!node)
			{
				return VisitorControl::kContinue;
			}

			for (auto object : node->m_children)
			{
				if (Traverse(object, a_func) == VisitorControl::kStop)
				{
					return VisitorControl::kStop;
				}
			}
		}

		template <class Tp>
		VisitorControl TraverseGeometry(NiAVObject* a_object, Tp a_func)
		{
			if (!a_object)
			{
				return VisitorControl::kContinue;
			}

			if (auto geometry = a_object->GetAsBSGeometry())
			{
				if (a_func(geometry) == VisitorControl::kStop)
				{
					return VisitorControl::kStop;
				}
			}

			auto node = a_object->GetAsNiNode();
			if (!node)
			{
				return VisitorControl::kContinue;
			}

			for (auto object : node->m_children)
			{
				if (TraverseGeometry(object, a_func) == VisitorControl::kStop)
				{
					return VisitorControl::kStop;
				}
			}

			return VisitorControl::kContinue;
		}

	}
}