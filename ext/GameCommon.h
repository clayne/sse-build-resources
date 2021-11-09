#pragma once

#include <skse64/GameReferences.h>
#include <skse64/GameMenus.h>

class BSTempEffect;
class SyncQueueObj;

namespace Game
{
    void AIProcessVisitActors(const std::function<void(Actor*)>& a_func);
    char GetActorSex(Actor* a_actor);
    TESRace* GetActorRace(Actor* a_actor);
    float GetNPCWeight(TESNPC* a_npc);
    float GetActorWeight(Actor* a_actor);

    void AIProcessVisitActors2(const std::function<void(Actor*, const Game::ActorHandle&)>& a_func, bool a_includePlayer = false);

    SKMP_FORCEINLINE bool InPausedMenu()
    {
        auto mm = MenuManager::GetSingleton();
        return mm && mm->InPausedMenu();
    }

    namespace Debug
    {
        void Notification(const char* a_message, bool a_cancelIfQueued = true, const char* a_sound = nullptr);
    }


    // https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/AI/ProcessLists.h
    class ProcessLists
    {
    public:

        static ProcessLists* GetSingleton();

        bool GuardsPursuing(Actor* a_actor);

        inline void ResetEffectShaders(const Game::ObjectRefHandle& a_handle)
        {
            return (this->*ResetEffectShaders_addr)(a_handle);
        }

        bool                                    runDetection;                                  // 001
        bool                                    showDetectionStats;                            // 002
        std::uint8_t                            pad003;                                        // 003
        ObjectRefHandle                         statdetect;                                    // 004
        bool                                    processHigh;                                   // 008
        bool                                    processLow;                                    // 009
        bool                                    processMHigh;                                  // 00A
        bool                                    processMLow;                                   // 00B
        std::uint16_t                           unk00C;                                        // 00C
        std::uint8_t                            unk00E;                                        // 00E
        std::uint8_t                            pad00F;                                        // 00F
        std::int32_t                            numberHighActors;                              // 010
        float                                   unk014;                                        // 014
        std::uint32_t                           unk018;                                        // 018
        float                                   removeExcessDeadTimer;                         // 01C
        std::uint64_t                           movementSyncSema;                              // 020
        std::uint32_t                           unk028;                                        // 028
        std::uint32_t                           pad02C;                                        // 02C
        tArray<ActorHandle>                     highActorHandles;                              // 030
        tArray<ActorHandle>                     lowActorHandles;                               // 048
        tArray<ActorHandle>                     middleHighActorHandles;                        // 060
        tArray<ActorHandle>                     middleLowActorHandles;                         // 078
        tArray<ActorHandle>* allProcesses[4];                               // 090
        void* globalCrimes[7];                               // 0B0
        tArray<NiPointer<BSTempEffect>>         globalTempEffects;                             // 0E8
        mutable SimpleLock                      globalEffectsLock;                             // 100
        tArray<NiPointer<BSTempEffect>>         magicEffects;                                  // 108
        mutable SimpleLock                      magicEffectsLock;                              // 120
        tArray<NiPointer<BSTempEffect>>         interfaceEffects;                              // 128
        mutable SimpleLock                      interfaceEffectsLock;                          // 140
        std::uint64_t                           unk148;                                        // 148
        std::uint64_t                           unk150;                                        // 150
        tArray<ObjectRefHandle>                 tempShouldMoves;                               // 158
        std::uint64_t                           unk170[2];                                     // 170
        tArray<ActorHandle>                     initPackageLocationsQueue;                     // 180
        mutable SimpleLock                      packageLocationsQueueLock;                     // 198
        tArray<ActorHandle>                     initAnimPositionQueue;                         // 1A0
        tArray<BSTSmartPointer<SyncQueueObj>>   syncPositionQueue;                             // 1B8
        float                                   playerActionCommentTimer;                      // 1D0
        float                                   playerKnockObjectCommentTimer;                 // 1D4
        std::uint32_t                           currentLowActor;                               // 1D8
        std::uint32_t                           currentMiddleHighActor;                        // 1DC
        std::uint32_t                           currentMiddleLowActor;                         // 1E0
        bool                                    runSchedules;                                  // 1E4
        bool                                    runMovement;                                   // 1E5
        bool                                    runAnimations;                                 // 1E6
        bool                                    updateActorsInPlayerCell;                      // 1E7
        std::uint64_t                           unk1E8;                                        // 1E8

    private:

        DEFINE_MEMBER_FN_LONG(ProcessLists, _GuardsPursuing, std::uint32_t, 40314, Actor* a_actor, int p2, char p3);

        inline static auto ResetEffectShaders_addr = IAL::Address<decltype(&ResetEffectShaders)&>(40378);
    };

    static_assert(offsetof(ProcessLists, highActorHandles) == 0x30);
    static_assert(offsetof(ProcessLists, globalTempEffects) == 0x0E8);
    static_assert(offsetof(ProcessLists, currentMiddleLowActor) == 0x1E0);
    static_assert(sizeof(ProcessLists) == 0x1F0);

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

    class InventoryChanges
    {
    public:
        class IItemChangeVisitor
        {
        public:
            inline static constexpr auto RTTI = RTTI_IID::InventoryChanges__IItemChangeVisitor;

            virtual ~IItemChangeVisitor();  // 00

            // add
            virtual bool Visit(InventoryEntryData* a_entryData) = 0;  // 01
            virtual void Unk_02(void);                                // 02 - { return 1; }
            virtual void Unk_03(void);                                // 03
        };
        static_assert(sizeof(IItemChangeVisitor) == 0x8);

    private:

    };

    class InitWornVisitor :
        public InventoryChanges::IItemChangeVisitor
    {
    public:

        virtual ~InitWornVisitor();                                     // 00

        virtual bool Visit(InventoryEntryData* a_entryData) override;   // 01
        virtual void Unk_02(void) override;                             // 02 
        virtual void Unk_03(void) override;                             // 03

        TESNPC* npc;
        Actor* actor;
        void* data; // root node
        TESRace* race;
    };

}