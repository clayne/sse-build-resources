#pragma once

#include <skse64/GameTypes.h>
#include <skse64/GameReferences.h>
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
    }
}