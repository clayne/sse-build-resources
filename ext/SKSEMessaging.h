#pragma once

namespace SKSEMessaging
{
    struct NodeUpdateMessage
    {
        enum Type : std::uint32_t
        {
            kMessage_NodeMoveComplete = 0x9C132B91
        };

        Game::FormID actor;
    };

    struct NodeListMessage
    {
        struct Entry
        {
            const char* name;
        };

        enum Type : std::uint32_t
        {
            kMessage_NodeList = 0x9C132B92
        };

        std::uint32_t size;
        Entry* list;
    };
}