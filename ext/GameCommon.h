#pragma once

#include <skse64/GameReferences.h>

namespace Game
{
    void AIProcessVisitActors(const std::function<void(Actor*)>& a_func);
    char GetActorSex(Actor* a_actor);
    TESRace* GetActorRace(Actor* a_actor);

    // https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/AI/ProcessLists.h
    class ProcessLists
    {
    public:

        static ProcessLists* GetSingleton();

        bool GuardsPursuing(Actor* a_actor);

        bool                                    runDetection;                                  // 001
        bool                                    showDetectionStats;                            // 002
        uint8_t                                 pad003;                                        // 003
        ObjectRefHandle                         statdetect;                                    // 004
        bool                                    processHigh;                                   // 008
        bool                                    processLow;                                    // 009
        bool                                    processMHigh;                                  // 00A
        bool                                    processMLow;                                   // 00B
        uint16_t                                unk00C;                                        // 00C
        uint8_t                                 unk00E;                                        // 00E
        uint8_t                                 pad00F;                                        // 00F
        int32_t                                 numberHighActors;                              // 010
        float                                   unk014;                                        // 014
        uint32_t                                unk018;                                        // 018
        float                                   removeExcessDeadTimer;                         // 01C
        uint64_t                                movementSyncSema;                              // 020
        uint32_t                                unk028;                                        // 028
        uint32_t                                pad02C;                                        // 02C
        tArray<ObjectRefHandle>                 highActorHandles;                              // 030
        tArray<ObjectRefHandle>                 lowActorHandles;                               // 048
        tArray<ObjectRefHandle>                 middleHighActorHandles;                        // 060
        tArray<ObjectRefHandle>                 middleLowActorHandles;                         // 078
        tArray<ObjectRefHandle>* allProcesses[4];                                              // 090

    private:

        DEFINE_MEMBER_FN_LONG(ProcessLists, _GuardsPursuing, uint32_t, 40314, Actor* a_actor, int p2, char p3);

    };

    static_assert(offsetof(ProcessLists, highActorHandles) == 0x30);
    static_assert(sizeof(ProcessLists) == 0xB0);

    struct PositionPlayerEvent;
    struct BSGamerProfileEvent;

    // https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/BSMain/Main.h
    class BSMain :
        BSTEventSink <PositionPlayerEvent>, // 00
        BSTEventSink <BSGamerProfileEvent>  // 08
    {
    public:
        virtual ~BSMain();

        virtual EventResult    ReceiveEvent(PositionPlayerEvent* evn, EventDispatcher<PositionPlayerEvent>* dispatcher) override;
        virtual EventResult    ReceiveEvent(BSGamerProfileEvent* evn, EventDispatcher<BSGamerProfileEvent>* dispatcher) override;

        static BSMain* GetSingleton();

        bool                         quitGame;                        // 010
        bool                         resetGame;                       // 011
        bool                         fullReset;                       // 012
        bool                         gameActive;                      // 013
        bool                         onIdle;                          // 014
        bool                         reloadContent;                   // 015
        bool                         freezeTime;                      // 016
        bool                         freezeNextFrame;                 // 017

    };

    static_assert(offsetof(BSMain, quitGame) == 0x10);
    static_assert(sizeof(BSMain) == 0x18);

    class Unk00
    {
    public:
        static Unk00* GetSingleton();

        DEFINE_MEMBER_FN_LONG(Unk00, SetGlobalTimeMultiplier, void, 66988, float a_scale, bool a_unk);

    };

}