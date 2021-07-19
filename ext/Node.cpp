
#include "Node.h"

namespace Util
{
    namespace Node
    {
        NiNode* FindNode(
            NiNode* a_root,
            const BSFixedString& a_name)
        {
            auto object = a_root->GetObjectByName(a_name);
            if (!object) {
                return nullptr;
            }

            return object->GetAsNiNode();
        }

        NiRootNodes::NiRootNodes(
            TESObjectREFR* const a_ref,
            bool a_no1p)
        {
            auto root3p = a_ref->GetNiRootNode(false);
            auto root1p = a_no1p ? nullptr : a_ref->GetNiRootNode(true);

            m_nodes[0] = root3p;

            if (root3p == root1p) {
                m_nodes[1] = nullptr;
            }
            else {
                m_nodes[1] = root1p;
            }
        }

        void NiRootNodes::GetNPCRoots(const BSFixedString& a_npcroot)
        {
            for (std::size_t i = 0; i < std::size(m_nodes); i++)
            {
                auto& root = m_nodes[i];

                if (!root) {
                    continue;
                }

                auto n = FindNode(root, a_npcroot);
                if (n) {
                    m_nodes[i] = n;
                }
            }
        }

        bool NiRootNodes::MatchesAny(
            NiNode* const a_node)
        {
            return a_node == m_nodes[0] || a_node == m_nodes[1];
        }
    }
}