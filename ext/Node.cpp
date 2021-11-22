
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
            if (!object)
            {
                return nullptr;
            }

            return object->GetAsNiNode();
        }

        NiAVObject* FindChild(
            NiNode* a_node,
            const BSFixedString& a_name)
        {
            for (auto object : a_node->m_children)
            {
                if (!object)
                {
                    continue;
                }

                if (object->m_name == a_name)
                {
                    return object;
                }
            }

            return nullptr;
        }

        NiNode* FindChildNode(
            NiNode* a_node,
            const BSFixedString& a_name)
        {
            for (auto object : a_node->m_children)
            {
                if (!object)
                {
                    continue;
                }

                if (object->m_name == a_name)
                {
                    if (auto node = object->GetAsNiNode())
                    {
                        return node;
                    }
                }
            }

            return nullptr;
        }

        NiRootNodes::NiRootNodes(
            TESObjectREFR* const a_ref,
            bool a_no1p)
        {
            auto root3p = a_ref->GetNiRootNode(false);
            auto root1p = a_no1p ? nullptr : a_ref->GetNiRootNode(true);

            m_nodes[0].reset(root3p);

            if (root3p == root1p)
            {
                m_nodes[1].reset();
            }
            else
            {
                m_nodes[1].reset(root1p);
            }
        }

        void NiRootNodes::GetNPCRoots(const BSFixedString& a_npcroot)
        {
            for (auto& root : m_nodes)
            {
                if (!root)
                {
                    continue;
                }

                if (auto n = FindNode(root, a_npcroot))
                {
                    root.reset(n);
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