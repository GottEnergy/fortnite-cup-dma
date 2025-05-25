
#include "..\kmbox\kmbox.hpp"
#include "..\settings.h"
#include <random>   


namespace aim {


    Vector3 FinalAimPosition = { 0.f, 0.f, 0.f };

    Vector3 predictLocation(Vector3 targetPos, Vector3 targetVel, float projectileSpeed, float gravity, float distance)
    {
        if (projectileSpeed <= 0.0f)
            return targetPos;

        // Initial guess of travel time (distance / speed)
        float travelTime = distance / projectileSpeed;

        // Iterate a few times to refine the travel time estimation
        for (int i = 0; i < 3; ++i)
        {
            // Predict the target's future position based on its velocity and the current time estimate
            Vector3 predictedPos = targetPos + targetVel * travelTime;

            // Recalculate the distance from the shooter to the predicted target position
            float newDistance = sqrt((predictedPos.x - targetPos.x) * (predictedPos.x - targetPos.x) +
                (predictedPos.y - targetPos.y) * (predictedPos.y - targetPos.y) +
                (predictedPos.z - targetPos.z) * (predictedPos.z - targetPos.z));

            // Refine the travel time based on the updated predicted distance
            travelTime = newDistance / projectileSpeed;
        }

        // Final prediction of the target position
        Vector3 predicted = targetPos + targetVel * travelTime;

        // Apply gravity (assuming gravity is negative like in Fortnite)
        predicted.z += 0.5f * gravity * travelTime * travelTime;

        return predicted;
    }





    bool isHit(Vector3 loc, double margin = 20) {
        if (mainCamera.LocationUnderReticle.x >= loc.x - margin && mainCamera.LocationUnderReticle.x <= loc.x + margin && mainCamera.LocationUnderReticle.y >= loc.y - margin && mainCamera.LocationUnderReticle.y <= loc.y + margin)
            return true;
        else
            return false;
    }

    bool isHit2D(Vector3 loc, double margin = 20) {
        if (settings::window::Width / 2 >= loc.x - margin && settings::window::Width / 2 <= loc.x + margin && settings::window::Height / 2 >= loc.y - margin && settings::window::Height / 2 <= loc.y + margin)
            return true;
        else
            return false;
    }

    double isClose(Vector3 loc2D) {

        const double maxDistance = std::sqrt(std::pow(settings::window::Width, 2) + std::pow(settings::window::Height, 2)) / 2.0;

        double distance = std::sqrt(std::pow(loc2D.x - settings::window::Width / 2, 2) + std::pow(loc2D.y - settings::window::Height / 2, 2));

        double closeness = 1.0f - (distance / maxDistance);

        closeness = std::clamp(closeness, 0.0, 1.0);

        return closeness;
    }

    constexpr float RadiansToDegrees(float radians) {
        return radians * (180.0f / M_PI);
    }

    bool anyHitboxSelected()
    {
        for (bool selected : settings::config::AimbotTargetHitboxes)
            if (selected) return true;
        return false;
    }

    Rotation targetRotation(const Vector3& currentPosition, const Vector3& targetPosition) {
        float directionX = targetPosition.x - currentPosition.x;
        float directionY = targetPosition.y - currentPosition.y;
        float directionZ = targetPosition.z - currentPosition.z;

        float yawRadians = std::atan2(directionY, directionX);
        float yawDegrees = RadiansToDegrees(yawRadians);

        float distanceXY = std::sqrt(directionX * directionX + directionY * directionY); // Horizontal distance
        float pitchRadians = std::atan2(directionZ, distanceXY);
        float pitchDegrees = RadiansToDegrees(pitchRadians);

        return { yawDegrees, pitchDegrees };
    }


    bool isVisible(const PlayerCache& player, float maxRenderDelay = 0.06f) {
        return (point::Seconds - player.last_render) <= maxRenderDelay;
    }


  


  




    PlayerCache target{};
    bool Targetting = false;

    void updateAimbot()
    {
        if (!settings::runtime::hotKeys ||
            (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox && !settings::config::MoonlightAim) ||
            !settings::config::Aimbot ||
            !point::Player || !point::PlayerState)
            return;

        if (!mem.IsKeyDown(settings::config::AimKey)) {
            target.PlayerState = 0;
            Targetting = false;
            return;
        }

        std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;
        if (PlayerList.empty()) return;

        double closest = HUGE;
        PlayerCache closestPlayer{};
        bool foundTarget = false;

        for (const auto& [_, player] : PlayerList) {
            if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;
            if (!isVisible(player) || player.isDying) continue;
            if (player.PlayerState == point::PlayerState || player.TeamId == local_player::localTeam) continue;

            double distance2D = std::sqrt(
                std::pow(player.Head2D.x - settings::window::Width / 2, 2) +
                std::pow(player.Head2D.y - settings::window::Height / 2, 2));

            if (distance2D < settings::config::AimFov && distance2D < closest) {
                closest = distance2D;
                closestPlayer = player;
                foundTarget = true;
            }
        }

        if (!foundTarget) {
            target.PlayerState = 0;
            Targetting = false;
            return;
        }

        target = closestPlayer;
        Targetting = true;

        // Aimbot Mode logic (Static, Random Bone, Nearest)
        enum class HitboxType { Head, Neck, Chest };
        static HitboxType lockedHitbox = HitboxType::Head;
        static std::chrono::steady_clock::time_point lastTargetTime = std::chrono::steady_clock::now();

        Vector3 aimPoint; // Default aimPoint in case no mode is selected
        switch (settings::config::AimModeSelection) {
        case 0: // Static (Head)
            aimPoint = closestPlayer.Head3D;
            break;

        case 1: // Random Bone
        {
            constexpr std::chrono::milliseconds aimChangeCooldown(5000);  // 5 seconds
            if (std::chrono::steady_clock::now() - lastTargetTime >= aimChangeCooldown) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> weightDist(1, 100);
                int roll = weightDist(gen);

                if (roll <= 30) {
                    lockedHitbox = HitboxType::Head;
                }
                else if (roll <= 45) {
                    lockedHitbox = HitboxType::Neck;
                }
                else {
                    lockedHitbox = HitboxType::Chest;
                }

                lastTargetTime = std::chrono::steady_clock::now();
            }

            switch (lockedHitbox) {
            case HitboxType::Head: aimPoint = closestPlayer.Head3D; break;
            case HitboxType::Neck: aimPoint = closestPlayer.Neck3D; break;
            case HitboxType::Chest: aimPoint = closestPlayer.Hip3D; break;
            }
            break;
        }

        case 2: // Nearest Hitbox (to the center of the screen)
        {
            // Camera position (or player position)
            Vector3 cameraPosition = mainCamera.Location;

            // Assume that we have a screen center, which is usually (screenWidth / 2, screenHeight / 2)
            float screenCenterX = settings::window::Width / 2.0f;
            float screenCenterY = settings::window::Height / 2.0f;

            float minDistance = FLT_MAX;
            Vector3 nearestBone = closestPlayer.Head3D; // Default to head

            // List of bones with their 3D positions
            std::vector<std::pair<Vector3*, std::string>> bones = {
                {&closestPlayer.Head3D, "Head"},
                {&closestPlayer.Bottom3D, "Bottom"},
                {&closestPlayer.Hip3D, "Hip"},
                {&closestPlayer.Neck3D, "Neck"},
                {&closestPlayer.UpperArmLeft3D, "UpperArmLeft"},
                {&closestPlayer.UpperArmRight3D, "UpperArmRight"},
                {&closestPlayer.LeftHand3D, "LeftHand"},
                {&closestPlayer.RightHand3D, "RightHand"},
                {&closestPlayer.LeftHandT3D, "LeftHandT"},
                {&closestPlayer.RightHandT3D, "RightHandT"},
                {&closestPlayer.RightThigh3D, "RightThigh"},
                {&closestPlayer.LeftThigh3D, "LeftThigh"},
                {&closestPlayer.RightCalf3D, "RightCalf"},
                {&closestPlayer.LeftCalf3D, "LeftCalf"},
                {&closestPlayer.LeftFoot3D, "LeftFoot"},
                {&closestPlayer.RightFoot3D, "RightFoot"}
            };

            // Iterate through each bone to find the nearest one to the screen center
            for (auto& [bone3D, boneName] : bones) {
                // Get the 3D coordinates of the bone
                Vector3 bone = *bone3D;

                // Calculate direction vector from camera to bone
                Vector3 toBone = bone - cameraPosition;

                // Simple 2D projection of the bone based on the direction vector
                // We simulate perspective by scaling based on the Z distance (using the formula for simple projection)
                float scale = 1.0f / toBone.z; // Basic perspective scaling

                // Simulate 2D screen position based on perspective
                float projectedX = screenCenterX + (toBone.x * scale) * settings::window::Width;
                float projectedY = screenCenterY - (toBone.y * scale) * settings::window::Height;

                // Calculate the 2D distance from the center of the screen
                float distance = std::sqrt(std::pow(projectedX - screenCenterX, 2) + std::pow(projectedY - screenCenterY, 2));

                // If this bone is closer to the center, select it
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestBone = bone;
                }
            }

            // Set the aim point to the nearest bone's 3D coordinates
            aimPoint = nearestBone;
            break;
             }



        }

        // Prediction
        if (settings::config::Prediction && point::ProjectileSpeed > 0) {
            aimPoint = predictLocation(
                aimPoint,
                closestPlayer.Velocity,
                offsets::WeaponProjectileSpeed,
                offsets::WeaponProjectileGravity,
                static_cast<float>(mainCamera.Location.Distance(aimPoint))
            );
            FinalAimPosition = aimPoint;
        }

        // Target rotation calculation
        Rotation targetRot = targetRotation(mainCamera.Location, aimPoint);
        float deltaYaw = targetRot.yaw - mainCamera.Rotation.y;
        float deltaPitch = targetRot.pitch - mainCamera.Rotation.x;

        while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
        while (deltaYaw < -180.0f) deltaYaw += 360.0f;
        deltaPitch = std::clamp(deltaPitch, -89.9f, 89.9f);

        constexpr float AIM_THRESHOLD = 0.01f;
        constexpr float BASE_MOUSE_STEP = 28.0f;
        constexpr float MIN_STEP = 0.3f;

        if (std::abs(deltaYaw) < AIM_THRESHOLD && std::abs(deltaPitch) < AIM_THRESHOLD)
            return;

        float smoothing = std::clamp(settings::config::AimSmoothing, 1.0f, 50.0f);
        float dynamicStep = std::clamp(BASE_MOUSE_STEP / smoothing, MIN_STEP, BASE_MOUSE_STEP);

        float moveXf = deltaYaw * dynamicStep;
        float moveYf = deltaPitch * -dynamicStep;

        float maxMoveX = std::abs(deltaYaw * BASE_MOUSE_STEP);
        float maxMoveY = std::abs(deltaPitch * BASE_MOUSE_STEP);

        moveXf = std::clamp(moveXf, -maxMoveX, maxMoveX);
        moveYf = std::clamp(moveYf, -maxMoveY, maxMoveY);

        static float mouseRemainderX = 0.0f, mouseRemainderY = 0.0f;
        float moveX = moveXf + mouseRemainderX;
        float moveY = moveYf + mouseRemainderY;

        int intMoveX = static_cast<int>(std::round(moveX));
        int intMoveY = static_cast<int>(std::round(moveY));

        mouseRemainderX = moveX - intMoveX;
        mouseRemainderY = moveY - intMoveY;

        if (intMoveX != 0 || intMoveY != 0) {
            if (settings::config::MoonlightAim) {
                mouse_event(MOUSEEVENTF_MOVE, intMoveX, intMoveY, 0, 0);
            }
            else if (settings::kmbox::NetKmbox) {
                kmNet_mouse_move(intMoveX, intMoveY);
            }
            else {
                km_move(intMoveX, intMoveY);
            }
        }
    }





    void updateTriggerBot()
    {
        if (!settings::runtime::hotKeys)
            return;

        if (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox && !settings::config::MoonlightAim)
            return;

        if (!settings::config::TriggerBot)
            return;

        static bool clicked = false;

        if (clicked) {
            if (settings::config::MoonlightAim)
                mouse_event(MOUSEEVENTF_LEFTUP, mainCamera.Rotation.x, mainCamera.Rotation.y, 0, 0);
            else
                kmNet_mouse_left(false);
            clicked = false;
        }

        static std::chrono::steady_clock::time_point lastClick = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastClick).count() < settings::config::TriggerDelay)
            return;

        if (mem.IsKeyDown(settings::config::TriggerKey)) {
            std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;

            for (auto it : PlayerList) {
                PlayerCache player = it.second;

                if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

                bool IsVis = offsets::Seconds - player.last_render <= 0.06f;

                if (player.isDying || !IsVis) continue;
                if (player.PlayerState == point::PlayerState) continue;
                if (player.TeamId == local_player::localTeam) continue;

                // Check selected hitboxes
                for (int i = 0; i < 16; i++) {
                    if (!settings::config::TargetHitboxes[i])
                        continue;

                    const Vector3* hitbox = nullptr;

                    switch (i) {
                    case 0:  hitbox = &player.Head3D; break;
                    case 1:  hitbox = &player.Bottom3D; break;
                    case 2:  hitbox = &player.Hip3D; break;
                    case 3:  hitbox = &player.Neck3D; break;
                    case 4:  hitbox = &player.UpperArmLeft3D; break;
                    case 5:  hitbox = &player.UpperArmRight3D; break;
                    case 6:  hitbox = &player.LeftHand3D; break;
                    case 7:  hitbox = &player.RightHand3D; break;
                    case 8:  hitbox = &player.LeftHandT3D; break;
                    case 9:  hitbox = &player.RightHandT3D; break;
                    case 10: hitbox = &player.RightThigh3D; break;
                    case 11: hitbox = &player.LeftThigh3D; break;
                    case 12: hitbox = &player.RightCalf3D; break;
                    case 13: hitbox = &player.LeftCalf3D; break;
                    case 14: hitbox = &player.LeftFoot3D; break;
                    case 15: hitbox = &player.RightFoot3D; break;
                    }

                    if (!hitbox) continue;

                    if (isHit(*hitbox)) {
                        lastClick = std::chrono::steady_clock::now();

                        if (settings::config::MoonlightAim) {
                            mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
                            clicked = true;
                        }
                        else if (settings::kmbox::NetKmbox) {
                            kmNet_mouse_left(true);
                            clicked = true;
                        }
                        else {
                            km_click();
                        }
                        break; // Only trigger once per player per frame
                    }
                }
            }
        }
    }
}

