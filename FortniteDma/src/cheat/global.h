#pragma once

#include "definitions.h"

#include <unordered_map>

namespace local_player {
    inline  Vector3 localPos = Vector3(); // not used
    inline uint32_t localTeam = NULL;
}

inline Camera mainCamera;

inline std::unordered_map<uintptr_t, PlayerCache> mainPlayerList; // global player cache

inline std::unordered_map<uintptr_t, PlayerCache> secondPlayerList;

namespace point {
    inline uintptr_t va_text = 0;
    inline uintptr_t Base = 0;
    inline uintptr_t Uworld = 0;
    inline uintptr_t Player = 0;
    inline uintptr_t PlayerState = 0;
    inline uintptr_t GameInstance = 0;
    inline uintptr_t PersistentLevel = 0;
    inline uintptr_t GameState = 0;
    inline uintptr_t Mesh = 0;
    inline uintptr_t RootComponent = 0;
    inline uintptr_t LocalPlayerState = 0;
    inline tarray PlayerArray{};
    inline uintptr_t ULevelArray = 0;
    inline uintptr_t ULevels = 0;
    inline uintptr_t AActorArray = 0;
    inline uintptr_t AActors = 0;
    inline uintptr_t LocalPlayers = 0;
    inline uintptr_t LocalPlayer = 0;
    inline uintptr_t PlayerController = 0;
    inline uintptr_t PlayerCameraManager = 0;
    inline uintptr_t LocationPointer = 0;
    inline uintptr_t RotationPointer = 0;
    inline uintptr_t CurrentWeapon = 0;
    inline uintptr_t Settings = 0;

    // values (not really points) should be elsewhere
    inline float ProjectileSpeed = 0;
    inline float ProjectileGravity = 0;

    inline float MouseSensX = 0.0025;
    inline float MouseSensY = 0.0025;

    // maybe move into main camera
    inline double Seconds = 0; // for vischeck
}

namespace info {

    namespace render {
        inline uint32_t playersRendered = 0; // players i am actually drawing
        inline uint32_t playersLooped = 0; // all players i am looping when rendering
        inline uint32_t validPlayersLooped = 0; // valid players i am looping when rendering
        inline uint32_t invalidPlayersLooped = 0; // valid players i am looping when rendering
        inline uint32_t teammatesSkipped = 0; // players in my team skipped when rendering
        inline uint32_t validBots = 0; // valid bots looped when rendering
    }

}