#pragma once

#include <skse64/GameTypes.h>
#include <skse64/GameReferences.h>
#include <skse64/NiNodes.h>

namespace Util
{
    namespace Node
    {
        NiNode* FindNode(
            NiNode* a_root,
            const BSFixedString& a_name);

        struct NiRootNodes
        {
            NiRootNodes(TESObjectREFR* const a_ref, bool a_no1p = false);

            [[nodiscard]] bool MatchesAny(NiNode* const a_node);

            void GetNPCRoots(const BSFixedString& a_npcroot);

            NiPointer<NiNode> m_nodes[2];
        };
    }
}