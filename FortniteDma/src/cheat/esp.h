#pragma once

namespace esp {
    bool shouldDisplayPlayer(PlayerCache player) {
        auto it = secondPlayerList.find(player.PlayerState);
        if (it != secondPlayerList.end()) {
            double dist = std::sqrt(std::pow(player.Head2D.x - settings::window::Width / 2, 2) + std::pow(player.Head2D.y - settings::window::Height / 2, 2));
            if (player.isDying || dist > settings::config::AimFov) {
                return false;
            }
        }
        else {
            return false;
        }
        return true;
    }

    float calculateYawTo(const Vector3& from, const Vector3& to) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        return atan2f(dy, dx) * (180.0f / 3.1415926f);
    }

    ImColor getDistanceColor(const PlayerCache& player, int alpha) {
        int distance = mainCamera.Location.Distance(player.Head3D) / 100;
        int maxDist = 300; // arbitrary scaling
        float t = std::clamp(distance / static_cast<float>(maxDist), 0.0f, 1.0f);
        int r = static_cast<int>(255 * t);
        int g = static_cast<int>(255 * (1.0f - t));
        return ImColor(r, g, 0, alpha);
    }


    struct Rotator {
        float Pitch;
        float Yaw;
        float Roll;

        Rotator() : Pitch(0.0f), Yaw(0.0f), Roll(0.0f) {}

        Rotator(float pitch, float yaw, float roll)
            : Pitch(pitch), Yaw(yaw), Roll(roll) {
        }

        std::string ToString() const {
            return "Pitch: " + std::to_string(Pitch) + " Yaw: " + std::to_string(Yaw) + " Roll: " + std::to_string(Roll);
        }
    };

    esp::Rotator getCameraRotation() {
        esp::Rotator cameraRot{};

        uintptr_t cameraManager = 0;

        // Step 1: Read the CameraManager pointer
        if (!mem.SPrepareEx(mem.hS4, point::PlayerController + offsets::Cameramanager, sizeof(cameraManager), &cameraManager))
            return cameraRot;

        // Step 2: Validate cameraManager isn't obviously invalid
        if (cameraManager < 0x10000 || cameraManager > 0x7FFFFFFFFFFF)
            return cameraRot;

        // Step 3: Read the actual rotation struct
        if (!mem.SPrepareEx(mem.hS4, cameraManager + offsets::CameraRotationPointer, sizeof(cameraRot), &cameraRot))
            return esp::Rotator(); // fallback zeroed value

        // Step 4: Sanity check values (to avoid NaN or garbage)
        if (!std::isfinite(cameraRot.Pitch) || !std::isfinite(cameraRot.Yaw) || !std::isfinite(cameraRot.Roll))
            return esp::Rotator();

        return cameraRot;
    }







    void drawArcIndicator(const PlayerCache& player, const ImVec2& screenCenter) {
        const float radius = 200.0f;
        const float arrowLength = 18.0f;

        if (!std::isfinite(player.Head3D.x) || !std::isfinite(player.Head3D.y) || !std::isfinite(player.Head3D.z)) return;
        if (!std::isfinite(mainCamera.Location.x) || !std::isfinite(mainCamera.Location.y) || !std::isfinite(mainCamera.Location.z)) return;

        // Direction vector to player
        Vector3 delta;
        delta.x = player.Head3D.x - mainCamera.Location.x;
        delta.y = player.Head3D.y - mainCamera.Location.y;
        delta.z = player.Head3D.z - mainCamera.Location.z;


        if (!std::isfinite(delta.x) || !std::isfinite(delta.y)) return;

        // World-space angle to player in XY plane (assuming X forward, Y right)
        float angleToPlayer = atan2f(delta.y, delta.x); // angle in radians

        // Get camera yaw and convert to radians
        esp::Rotator cameraRot = getCameraRotation();
        float cameraYawRad = cameraRot.Yaw * (M_PI / 180.0f);

        // Convert camera Yaw from Unreal (X forward) to angle matching atan2(y, x)
        float relativeAngle = angleToPlayer - cameraYawRad;

        // Normalize to [-π, π]
        while (relativeAngle > M_PI) relativeAngle -= 2.0f * M_PI;
        while (relativeAngle < -M_PI) relativeAngle += 2.0f * M_PI;

        // Draw position
        float x = screenCenter.x + cosf(relativeAngle) * radius;
        float y = screenCenter.y + sinf(relativeAngle) * radius;
        if (!std::isfinite(x) || !std::isfinite(y)) return;

        float time = ImGui::GetTime();
        float alpha = 100.0f * 0.5f * (1.0f + cosf(time * 2.0f * M_PI / 4.0f));
        alpha = std::clamp(alpha, 0.0f, 255.0f);

        ImColor arrowColor(255, 165, 0, static_cast<int>(alpha));
        ImColor outlineColor(0, 0, 0, static_cast<int>(alpha * 0.5f));

        ImVec2 p1(x, y);
        ImVec2 p2(x + cosf(relativeAngle + 0.5f) * arrowLength, y + sinf(relativeAngle + 0.5f) * arrowLength);
        ImVec2 p3(x + cosf(relativeAngle - 0.5f) * arrowLength, y + sinf(relativeAngle - 0.5f) * arrowLength);

        if (ImGui::GetCurrentContext()) {
            auto* drawList = ImGui::GetBackgroundDrawList();
            drawList->AddTriangleFilled(p1, p2, p3, outlineColor);
            drawList->AddTriangleFilled(p1, p2, p3, arrowColor);
        }
    }


    std::string get_weapon_name(uintptr_t Player)
    {
        std::string PlayersWeaponName = "";
        uint64_t CurrentWeapon = 0;

        // Step 1: Read the CurrentWeapon pointer
        if (!mem.SPrepareEx(mem.hS4, Player + offsets::CurrentWeapon, sizeof(CurrentWeapon), &CurrentWeapon))
            return "No Weapon";

        // Step 2: Validate pointer range
        if (CurrentWeapon < 0x10000 || CurrentWeapon > 0x7FFFFFFFFFFF)
            return "No Weapon";

        uint64_t weapondata = 0;
        if (!mem.SPrepareEx(mem.hS4, CurrentWeapon + offsets::WeaponData, sizeof(weapondata), &weapondata))
            return "No Weapon";

        if (weapondata < 0x10000 || weapondata > 0x7FFFFFFFFFFF)
            return "No Weapon";

        uint64_t ItemName = 0;
        if (!mem.SPrepareEx(mem.hS4, weapondata + 0x40, sizeof(ItemName), &ItemName))
            return "No Weapon";

        if (ItemName < 0x10000 || ItemName > 0x7FFFFFFFFFFF)
            return "No Weapon";

        uint64_t FData = 0;
        if (!mem.SPrepareEx(mem.hS4, ItemName + 0x20, sizeof(FData), &FData))
            return "No Weapon";

        int FLength = 0;
        if (!mem.SPrepareEx(mem.hS4, ItemName + 0x28, sizeof(FLength), &FLength))
            return "No Weapon";

        if (FLength > 0 && FLength < 50)
        {
            wchar_t* WeaponBuffer = new wchar_t[FLength];

            if (mem.SPrepareEx(mem.hS4, FData, FLength * sizeof(wchar_t), WeaponBuffer))
            {
                std::wstring wstr_buf(WeaponBuffer, FLength);
                PlayersWeaponName = std::string(wstr_buf.begin(), wstr_buf.end());
            }

            delete[] WeaponBuffer;
        }

        return PlayersWeaponName;
    }












    void renderPlayers() {
        int playersRendered = 0;
        int playersLooped = 0;
        int validPlayersLooped = 0;
        int invalidPlayersLooped = 0;
        int teammatesSkipped = 0;
        int bots = 0;

        for (auto it : secondPlayerList) {
            PlayerCache player = it.second;

            if (player.ignore)
                continue;

            playersLooped++;

            if (!isPlayerValid(player)) {
                invalidPlayersLooped++;
                continue;
            }

            validPlayersLooped++;

            bool IsVis = point::Seconds - player.last_render <= 0.06f;

            if (player.isBot) {
                bots++;
            }

            if (player.PlayerState == point::PlayerState) {
                continue;
            }

            if (point::Player && player.TeamId == local_player::localTeam) {
                teammatesSkipped++;
                continue;
            }

            if (player.bisDying) {
                continue;
            }

            int distanceMeters = mainCamera.Location.Distance(player.Head3D) / 100;

            playersRendered++;

            if (settings::config::Skeleton) {
                ImColor colSK = ImColor(
                    settings::config::SkeletonColor[0],
                    settings::config::SkeletonColor[1],
                    settings::config::SkeletonColor[2],
                    settings::config::SkeletonColor[3]
                );

                if (IsVis)
                    colSK = ImColor(
                        settings::config::SkeletonColor[0],
                        settings::config::SkeletonColor[1],
                        settings::config::SkeletonColor[2],
                        settings::config::SkeletonColor[3]
                    );

                float tk = 0.1f;

                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Neck2D.x, player.Neck2D.y), ImVec2(player.Head2D.x, player.Head2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Hip2D.x, player.Hip2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.LeftHandT2D.x, player.LeftHandT2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.RightHandT2D.x, player.RightHandT2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftFoot2D.x, player.LeftFoot2D.y), ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), colSK, tk);
                ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightFoot2D.x, player.RightFoot2D.y), ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), colSK, tk);
            }


            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            ImVec2 Circle_coord{ screenSize.x / 2.0f, screenSize.y / 2.0f };

            if (settings::config::Box) {
                ImColor boxColor = ImColor(settings::config::BoxColor[0],
                    settings::config::BoxColor[1],
                    settings::config::BoxColor[2],
                    settings::config::BoxColor[3]);

                float tk = 1.f;
                float rd = 0.f;

                float box_height = abs(player.Top2D.y - player.Bottom2D.y);
                float box_width = 0.35f * box_height;

                ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
                ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

                ImGui::GetBackgroundDrawList()->AddRect(topLeft, bottomRight, boxColor, rd, 0, tk);
            }

            if (settings::config::Indicators) {
                drawArcIndicator(player, Circle_coord);
            }

            if (settings::config::Distance) {
                ImColor colTxt = ImColor(255, 255, 255, 255);

                float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
                float box_width = 0.5f * box_height;

                ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
                ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

                string distanceStr = std::format("({:d}m)", distanceMeters);
                ImVec2 distanceSize = ImGui::CalcTextSize(distanceStr.c_str());
                ImVec2 distancePos = ImVec2(
                    topLeft.x + (box_width / 2.0f) - (distanceSize.x / 2.0f),
                    bottomRight.y + 5.0f
                );

                ImGui::GetBackgroundDrawList()->AddText(distancePos, colTxt, distanceStr.c_str());
            }
        }

       



        if (settings::config::Aimbot && settings::config::Prediction && aim::Targetting) {
            if (point::ProjectileSpeed > 0 && shouldDisplayPlayer(aim::target)) {
                Vector3 pred = w2s(aim::predictLocation(aim::target.Head3D, aim::target.Velocity, point::ProjectileSpeed, point::ProjectileGravity, (float)mainCamera.Location.Distance(aim::target.Head3D)));
                ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(pred.x, pred.y), 5, ImColor(255, 255, 255, 255), 20);
            }
        }

        // stats
        info::render::playersRendered = playersRendered;
        info::render::playersLooped = playersLooped;
        info::render::validPlayersLooped = validPlayersLooped;
        info::render::invalidPlayersLooped = invalidPlayersLooped;
        info::render::teammatesSkipped = teammatesSkipped;
        info::render::validBots = bots;
    }

    void Debug() {
        const auto& MainValues = stats::mainThreadData.getValues();
        float mainAvgMs = std::accumulate(MainValues.begin(), MainValues.end(), 0.0f) / MainValues.size();
        float mainFPS = (mainAvgMs > 0) ? (1000.0f / mainAvgMs) : 0.0f;
        string mainStr = std::format("FPS: {:.0f}", mainFPS);

        if (settings::config::showFps) {
            ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 255, 255, 255), mainStr.c_str());
        }
    }
}