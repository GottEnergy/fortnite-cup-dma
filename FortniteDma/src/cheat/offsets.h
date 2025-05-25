#pragma once
#include <cstdint>

namespace offsets {
    // Globals
    uintptr_t Uworld = 0x17AA2698;
    uintptr_t RootComponent = 0x1B0;
    uintptr_t GameInstance = 0x238;
    uintptr_t GameState = 0x1C0;
    uintptr_t PersistentLevel = 0x40;
    uintptr_t LocalPlayers = 0x38;
    uintptr_t PlayerController = 0x30;
    uintptr_t LocalPawn = 0x350;
    uintptr_t Cameramanager = 0x360;
    uintptr_t PlayerState = 0x2C8;
    uintptr_t PawnPrivate = 0x320;
    uintptr_t PlayerArray = 0x2C0;
    uintptr_t Mesh = 0x328;
    uintptr_t TeamId = 0x1251;
    uintptr_t ComponentToWorld = 0x1E0;
    uintptr_t Velocity = 0x180;

    // Visibility
    uintptr_t Seconds = 0x188;
    uintptr_t LastRenderTime = 0x32c;

    // Camera
    uintptr_t CameraLocationPointer = 0x168;
    uintptr_t CameraRotationPointer = 0x178;
    uintptr_t FieldOfView = 0x4AC;

    // Bone
    uintptr_t BoneArray = 0x5E8;

    // Health/Status
    uintptr_t IsDBNO = 0x98A;
    uintptr_t isDying = 0x728;
    uintptr_t bIsABot = 0x2b2;

    // Reticle/Targeting
    uintptr_t LocationUnderReticle = 0x2DD8;

    // Weapon
    uintptr_t WeaponData = 0x618;
    uintptr_t CurrentWeapon = 0x730;
    uintptr_t WeaponCoreAnimation = 0x1730;
    uintptr_t WeaponProjectileSpeed = 0x1f94;
    uintptr_t WeaponProjectileGravity = 0x1F98;

    // ESP Loots
    uintptr_t OwningWorld = 0x200;
    uintptr_t ULevelArray = 0x1D8;
    uintptr_t ULevelCount = ULevelArray + 8;
    uintptr_t AActorArray = 0x1E8;

    // Player Flags
    uintptr_t HabaneroComponent = 0xA48;
    uintptr_t PlayerName = 0xb08;
    uintptr_t KillScore = 0x1268;
    uintptr_t RankProgress = 0xd0;
    uintptr_t Platform = 0x430;

    // Spectators
    uintptr_t Spectators = 0xb80;
    uintptr_t SpectatorArray = 0x108;
    uintptr_t SpectatorCount = 0x8;
}
