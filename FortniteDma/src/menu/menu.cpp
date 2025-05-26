
#include "menu.h"

#include <fstream>
#include <iostream>
#include <string>
#include <format>
#include <numeric>

#include "../kmbox/kmboxNet.h"
#include "../kmbox/HidTable.h"
#include "../kmbox/kmbox_interface.h"

#include "../console/console.h"
#include "../cheat/global.h"
#include "../performance.h"
#include "../rendering/Font.h"
#include "../rendering/images.h"
#include "../settings.h"
#include "../kmbox/kmbox_util.h"

#include "../dma/DMAHandler.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "../cheat/utils.h"

// what is this you ask? well, ask chatgpt :)
static int StringInputTextCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		std::string* str = static_cast<std::string*>(data->UserData);
		str->resize(data->BufTextLen);
		data->Buf = &(*str)[0];
	}
	return 0;
}

constexpr float RadiansToDegrees(float radians) {
	return radians * (180.0f / M_PI);
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



void menu::Menu() {
	const float windowWidth = 850.0f;
	const float windowHeight = 548.0f;
	const float headerHeight = 28.0f;

	ImGui::SetNextWindowPos(ImVec2(100, 70), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.07f, 0.07f, 0.09f, 0.95f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.25f, 0.8f));

	ImGui::Begin("MainMenu", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar
	);

	ImVec2 fullSize = ImGui::GetWindowSize();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.07f, 0.09f, 0.95f));
	ImGui::BeginChild("HeaderBar", ImVec2(fullSize.x, headerHeight), false,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::SetCursorPosX(10.0f); // Left aligned
	ImGui::Text("Mental.ill - Substance Abuse Edition");
	ImGui::EndChild();
	ImGui::PopStyleColor();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.ChildRounding = 4.0f;
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.85f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.75f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 0.75f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.9f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.9f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);

	static int currentTab = 0;
	const char* tabs[] = { "Aim", "Visuals", "Playerlist", "Misc" };
	static bool selectingAim = false;
	static bool selectingTrigger = false;

	ImGui::BeginChild("##tabbar", ImVec2(140, 0), true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		ImVec4 btnColor = (i == currentTab)
			? ImVec4(0.25f, 0.25f, 0.35f, 0.95f)
			: style.Colors[ImGuiCol_Button];
		ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
		if (ImGui::Button(tabs[i], ImVec2(-1, 45)))
			currentTab = i;
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::Spacing();
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("##content", ImVec2(avail.x, avail.y), true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

	// --- Tab 0: Aim ---
	if (currentTab == 0) {
		ImGui::Text("Aimbot");
		ImGui::Checkbox("Aimbot", &settings::config::Aimbot);
		ImGui::SliderFloat("Smooth", &settings::config::AimSmoothing, 1.1f, 50.f);
		ImGui::SliderInt("FOV", &settings::config::AimFov, 1, settings::window::Height / 2);
		ImGui::Checkbox("Prediction", &settings::config::Prediction);
		ImGui::SliderFloat("Prediction Multiplier", &settings::config::PredictionMultiplier, 0.5f, 2.f);

		const char* aimModeOptions[] = { "Static", "Random Bone", "Nearest" };
		if (ImGui::BeginCombo("Aimbot Bone Mode", aimModeOptions[settings::config::AimModeSelection])) {
			for (int i = 0; i < IM_ARRAYSIZE(aimModeOptions); ++i) {
				bool selected = (settings::config::AimModeSelection == i);
				if (ImGui::Selectable(aimModeOptions[i], selected))
					settings::config::AimModeSelection = i;
				if (selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		const char* hitboxLabels[] = {
			"Head", "Bottom", "Hip", "Neck", "Upper Arm Left", "Upper Arm Right",
			"Left Hand", "Right Hand", "Left Hand Thumb", "Right Hand Thumb",
			"Right Thigh", "Left Thigh", "Right Calf", "Left Calf", "Left Foot", "Right Foot"
		};

		if (ImGui::BeginCombo("Aimbot Hitboxes", "Select...")) {
			for (int i = 0; i < IM_ARRAYSIZE(hitboxLabels); i++) {
				ImGui::Selectable(hitboxLabels[i], &settings::config::AimbotTargetHitboxes[i], ImGuiSelectableFlags_DontClosePopups);
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button(selectingAim ? "Press key" : std::format("Aim Key: {:d}", settings::config::AimKey).c_str()))
			if (!selectingTrigger) selectingAim = !selectingAim;
		if (selectingAim) {
			for (int i = 0; i < 256; i++) {
				if (mem.IsKeyDown(i)) {
					settings::config::AimKey = i;
					selectingAim = false;
				}
			}
		}

		ImGui::Separator();
		ImGui::Text("TriggerBot");
		ImGui::Checkbox("TriggerBot", &settings::config::TriggerBot);
		ImGui::SliderInt("Delay", &settings::config::TriggerDelay, 1, 100);

		if (ImGui::BeginCombo("Target Hitboxes", "Select...")) {
			for (int i = 0; i < IM_ARRAYSIZE(hitboxLabels); i++) {
				ImGui::Selectable(hitboxLabels[i], &settings::config::TargetHitboxes[i], ImGuiSelectableFlags_DontClosePopups);
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button(selectingTrigger ? "Press key" : std::format("Trg Key: {:d}", settings::config::TriggerKey).c_str()))
			if (!selectingAim) selectingTrigger = !selectingTrigger;
		if (selectingTrigger) {
			for (int i = 0; i < 256; i++) {
				if (mem.IsKeyDown(i)) {
					settings::config::TriggerKey = i;
					selectingTrigger = false;
				}
			}
		}
	}

	// --- Tab 1: Visuals ---
	else if (currentTab == 1) {
		ImGui::Text("ESP");
		ImGui::Checkbox("Box", &settings::config::Box);
		ImGui::SameLine();
		ImGui::ColorEdit4("##BoxColor", (float*)&settings::config::BoxColor, ImGuiColorEditFlags_NoInputs);

		ImGui::Checkbox("Skeleton", &settings::config::Skeleton);
		ImGui::SameLine();
		ImGui::ColorEdit4("##SkeletonColor", (float*)&settings::config::SkeletonColor, ImGuiColorEditFlags_NoInputs);
		ImGui::Checkbox("Distance", &settings::config::Distance);
		ImGui::Checkbox("Show FOV", &settings::config::ShowAimFov);
		ImGui::Checkbox("Crosshair", &settings::config::swastika); // Rename this for professionalism.
		ImGui::Checkbox("Show Prediction", &settings::config::renderPrediction);
		ImGui::Checkbox("Indicators", &settings::config::Indicators);

		ImGui::Separator();
		ImGui::Text("Radar");
		ImGui::Checkbox("Radar", &settings::config::Radar);
		settings::config::RadarXSize = settings::config::RadarYSize;
		ImGui::SliderFloat("Zoom", &settings::config::RadarZoom, 0.5f, 10.f);
	}

	// --- Tab 2: Playerlist ---
	else if (currentTab == 2) {
		std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;
		static uintptr_t selectedPlayer = 0;

		// Full area
		ImVec2 listSize = ImVec2(250, avail.y);
		ImVec2 detailSize = ImVec2(avail.x - listSize.x - 10, avail.y); // 10px spacing

		ImGui::BeginChild("##PlayerListMain", avail, true);

		// Left: Player list
		ImGui::BeginChild("##PlayerListLeft", listSize, true);
		for (auto& [stateAddr, cache] : PlayerList) {
			bool isSelected = (selectedPlayer == stateAddr);
			bool invalid = (!cache.Pawn || !cache.BoneArray);

			ImGui::PushStyleColor(ImGuiCol_Text, invalid
				? ImColor(255, 0, 0, 255).Value
				: ImColor(0, 255, 0, 255).Value);

			if (ImGui::Selectable(std::format("PlayerState -> 0x{:X} {} {}",
				stateAddr,
				invalid ? "(invalid)" : "(valid)",
				stateAddr == point::PlayerState ? "(me)" : ""
			).c_str(), isSelected)) {
				selectedPlayer = stateAddr;
			}

			ImGui::PopStyleColor();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// Right: Player details
		ImGui::BeginChild("##PlayerListRight", detailSize, true);

		if (selectedPlayer && PlayerList.contains(selectedPlayer)) {
			auto& p = PlayerList[selectedPlayer];
			uintptr_t BoneArray = p.BoneArrays[0].data;
			if (!BoneArray) BoneArray = p.BoneArrays[1].data;

			ImGui::Text("Selected: 0x%lX", selectedPlayer);
			ImGui::Separator();
			ImGui::Text("Pawn: 0x%lX", p.Pawn);
			ImGui::Text("Mesh: 0x%lX", p.Mesh);
			ImGui::Text("BoneArray: 0x%lX", BoneArray);
			ImGui::Text("RootComponent: 0x%lX", p.RootComponent);
			ImGui::Separator();

			ImGui::Text("Team ID: %d", p.TeamId);
			ImGui::Text("isDying: %s", (p.isDying & 0x10) ? "true" : "false");
			ImGui::Text("isDBNO: %s", (p.isDBNO & 0x10) ? "true" : "false");
			ImGui::Text("isBot: %s", (p.isBot & (1 << 3)) ? "true" : "false");
			ImGui::Separator();

			ImGui::Text("Velocity: x: %.2f y: %.2f z: %.2f", p.Velocity.x, p.Velocity.y, p.Velocity.z);
			ImGui::Text("Last Render: %.2f", p.last_render);
			ImGui::Separator();

			ImGui::Text("World Positions:");
			ImGui::Text("Head:     x: %.2f y: %.2f z: %.2f", p.Head3D.x, p.Head3D.y, p.Head3D.z);
			ImGui::Text("Bottom:   x: %.2f y: %.2f z: %.2f", p.Bottom3D.x, p.Bottom3D.y, p.Bottom3D.z);
			ImGui::Text("Hip:      x: %.2f y: %.2f z: %.2f", p.Hip3D.x, p.Hip3D.y, p.Hip3D.z);
			ImGui::Text("Neck:     x: %.2f y: %.2f z: %.2f", p.Neck3D.x, p.Neck3D.y, p.Neck3D.z);
			ImGui::Separator();

			ImGui::Text("Screen Positions:");
			ImGui::Text("Head:     x: %.2f y: %.2f z: %.2f", p.Head2D.x, p.Head2D.y, p.Head2D.z);
			ImGui::Text("Bottom:   x: %.2f y: %.2f z: %.2f", p.Bottom2D.x, p.Bottom2D.y, p.Bottom2D.z);
			ImGui::Text("Hip:      x: %.2f y: %.2f z: %.2f", p.Hip2D.x, p.Hip2D.y, p.Hip2D.z);
			ImGui::Text("Neck:     x: %.2f y: %.2f z: %.2f", p.Neck2D.x, p.Neck2D.y, p.Neck2D.z);
		}

		ImGui::EndChild();
		ImGui::EndChild();
	}


	// --- Tab 3: Misc ---
	else if (currentTab == 3) {
		ImGui::Text("Net");
		ImGui::InputText("Ip", &settings::kmbox::net::ip[0], settings::kmbox::net::ip.capacity() + 1, ImGuiInputTextFlags_CallbackResize, StringInputTextCallback, &settings::kmbox::net::ip);
		ImGui::InputText("Port", &settings::kmbox::net::port[0], settings::kmbox::net::port.capacity() + 1, ImGuiInputTextFlags_CallbackResize, StringInputTextCallback, &settings::kmbox::net::port);
		ImGui::InputText("Uuid", &settings::kmbox::net::uuid[0], settings::kmbox::net::uuid.capacity() + 1, ImGuiInputTextFlags_CallbackResize, StringInputTextCallback, &settings::kmbox::net::uuid);

		if (ImGui::Button("Connect")) {
			if (connect_net_kmbox()) {
				settings::kmbox::NetKmbox = true;
				settings::kmbox::net::saveConfig();
			}
		}

		ImGui::Separator();
		ImGui::Text("Misc");
		ImGui::Checkbox("Fuser Mode", &settings::config::Fuser);
		ImGui::Checkbox("vSync", &settings::config::vSync);
		ImGui::Checkbox("Show FPS", &settings::config::showFps);

		ImGui::Separator();
		if (ImGui::Button("Save")) settings::saveConfig();
		ImGui::SameLine();
		if (ImGui::Button("Load")) settings::loadConfig();

		ImGui::Separator();
		if (ImGui::Button("Close Cheat")) exit(0);
	}

	ImGui::EndChild(); // ##content
	ImGui::End();      // MainMenu

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
}







void menu::InternalsWindow() {
	ImVec2 size = ImVec2(500, 350);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Internals (Dev Only)", NULL, ImGuiWindowFlags_NoResize)) {

		ImGui::Separator();
		ImGui::Checkbox("GData", &settings::cheat::GData);
		ImGui::Separator();
		ImGui::Checkbox("Camera", &settings::cheat::Camera);
		ImGui::Separator();
		ImGui::Checkbox("PlayerList", &settings::cheat::PlayerList);
		ImGui::Separator();
		ImGui::Checkbox("Players", &settings::cheat::Players);
		ImGui::Separator();

	} ImGui::End();
}

void menu::StatisticsWindow() {
	ImVec2 size = ImVec2(500, 500);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Statistics", NULL, ImGuiWindowFlags_NoResize)) {

		// MainUpdate
		const auto& PlayersValues = stats::updatePlayersData.getValues();
		float PlayersAvgMs = std::accumulate(PlayersValues.begin(), PlayersValues.end(), 0.0f) / PlayersValues.size();
		float PlayersMaxMs = PlayersValues.empty() ? 0.0f : *std::max_element(PlayersValues.begin(), PlayersValues.end());

		ImGui::Text("Main Update performance Graph (us per execution)");
		ImGui::PlotLines("##Performance", PlayersValues.data(), PlayersValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::Text("Average: %.2f us Max: %.2f us", PlayersAvgMs, PlayersMaxMs);

		ImGui::Separator();

		// main thread
		const auto& MainValues = stats::mainThreadData.getValues();
		float mainAvgMs = std::accumulate(MainValues.begin(), MainValues.end(), 0.0f) / MainValues.size();
		float mainFPS = (mainAvgMs > 0) ? (1000.0f / mainAvgMs) : 0.0f;

		ImGui::Text("Main thread performance Graph (ms per loop)");
		ImGui::PlotLines("##Performance", MainValues.data(), MainValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		//ImGui::Text("Average: %.2f ms (%.1f FPS)", mainAvgMs, mainFPS);

		ImGui::Separator();

		static bool advanced = false;

		ImGui::Checkbox("Advanced view", &advanced);

		ImGui::Separator();

		if (advanced) {
			// GData
			const auto& GDataValues = stats::GDataData.getValues();
			float GDataAvgMs = std::accumulate(GDataValues.begin(), GDataValues.end(), 0.0f) / GDataValues.size();
			float GDataMaxMs = GDataValues.empty() ? 0.0f : *std::max_element(GDataValues.begin(), GDataValues.end());

			ImGui::Text("GDatad performance Graph (ms per execution)");
			ImGui::PlotLines("##Performance", GDataValues.data(), GDataValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", GDataAvgMs, GDataMaxMs);

			ImGui::Separator();

			// PlayerList
			const auto& PlayerListValues = stats::updatePlayerListData.getValues();
			float PlayerListAvgMs = std::accumulate(PlayerListValues.begin(), PlayerListValues.end(), 0.0f) / PlayerListValues.size();
			float PlayerListMaxMs = PlayerListValues.empty() ? 0.0f : *std::max_element(PlayerListValues.begin(), PlayerListValues.end());

			ImGui::Text("PlayerList performance Graph (ms per execution)");
			ImGui::PlotLines("##Performance", PlayerListValues.data(), PlayerListValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", PlayerListAvgMs, PlayerListMaxMs);
		}

	} ImGui::End();
}

void menu::PlayerListWindow() {
	ImVec2 size = ImVec2(510, 750);

	std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;
	static uintptr_t selectedPlayer = 0;

	// player list
	ImGui::SetNextWindowSize(size);

	if (ImGui::Begin("Player List", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::SetCursorPos(ImVec2(10, 30));
		ImGui::BeginChild("Player List", ImVec2(490, 710), ImGuiChildFlags_Border);

		for (auto it : PlayerList) {

			bool isSelected = (selectedPlayer == it.first);
			bool invalid = (!it.second.Pawn || !it.second.BoneArray);

			ImGui::PushStyleColor(ImGuiCol_Text, invalid ? ImColor(255, 0, 0, 255).Value : ImColor(0, 255, 0, 255).Value);

			if (ImGui::Selectable(std::format("PlayerState -> 0x{:X} {} {} ", it.first, invalid ? "(invalid)" : "(valid)", it.first == point::PlayerState ? "(me)" : "").c_str())) {
				selectedPlayer = it.first;
			}

			ImGui::PopStyleColor();

		}

		ImGui::EndChild();
	}ImGui::End();

	// details window
	if (selectedPlayer && PlayerList.find(selectedPlayer) != PlayerList.end()) {
		ImGui::SetNextWindowSize(ImVec2(400, 350));
		if (ImGui::Begin(std::format("Player Details for -> 0x{:X}", selectedPlayer).c_str(), NULL, ImGuiWindowFlags_NoResize)) {
			ImGui::Separator();
			ImGui::Text(std::format("Pawn -> 0x{:X}", PlayerList[selectedPlayer].Pawn).c_str());
			ImGui::Text(std::format("Mesh -> 0x{:X}", PlayerList[selectedPlayer].Mesh).c_str());
			uintptr_t BoneArray = PlayerList[selectedPlayer].BoneArrays[0].data;
			if (!BoneArray) BoneArray = PlayerList[selectedPlayer].BoneArrays[1].data;
			ImGui::Text(std::format("BoneArray -> 0x{:X}", BoneArray).c_str());
			ImGui::Text(std::format("RootComponent -> 0x{:X}", PlayerList[selectedPlayer].RootComponent).c_str());
			ImGui::Separator();
			ImGui::Text(std::format("Team -> {:d}", PlayerList[selectedPlayer].TeamId).c_str());
			ImGui::Text(std::format("isDying -> {}", PlayerList[selectedPlayer].isDying & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isDBNO -> {}", PlayerList[selectedPlayer].isDBNO & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isBot -> {}", PlayerList[selectedPlayer].isBot & (1 << 3) ? "true" : "false").c_str());
			ImGui::Separator();
			ImGui::Text(std::format("Velocity -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Velocity.x, PlayerList[selectedPlayer].Velocity.y, PlayerList[selectedPlayer].Velocity.z).c_str());
			ImGui::Text(std::format("Last Render -> {:.2f}", PlayerList[selectedPlayer].last_render).c_str());
			ImGui::Separator();
			ImGui::Text("World Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head3D.x, PlayerList[selectedPlayer].Head3D.y, PlayerList[selectedPlayer].Head3D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom3D.x, PlayerList[selectedPlayer].Bottom3D.y, PlayerList[selectedPlayer].Bottom3D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip3D.x, PlayerList[selectedPlayer].Hip3D.y, PlayerList[selectedPlayer].Hip3D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck3D.x, PlayerList[selectedPlayer].Neck3D.y, PlayerList[selectedPlayer].Neck3D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft3D.x, PlayerList[selectedPlayer].UpperArmLeft3D.y, PlayerList[selectedPlayer].UpperArmLeft3D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight3D.x, PlayerList[selectedPlayer].UpperArmRight3D.y, PlayerList[selectedPlayer].UpperArmRight3D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand3D.x, PlayerList[selectedPlayer].LeftHand3D.y, PlayerList[selectedPlayer].LeftHand3D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand3D.x, PlayerList[selectedPlayer].RightHand3D.y, PlayerList[selectedPlayer].RightHand3D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT3D.x, PlayerList[selectedPlayer].LeftHandT3D.y, PlayerList[selectedPlayer].LeftHandT3D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT3D.x, PlayerList[selectedPlayer].RightHandT3D.y, PlayerList[selectedPlayer].RightHandT3D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh3D.x, PlayerList[selectedPlayer].RightThigh3D.y, PlayerList[selectedPlayer].RightThigh3D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh3D.x, PlayerList[selectedPlayer].LeftThigh3D.y, PlayerList[selectedPlayer].LeftThigh3D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf3D.x, PlayerList[selectedPlayer].RightCalf3D.y, PlayerList[selectedPlayer].RightCalf3D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf3D.x, PlayerList[selectedPlayer].LeftCalf3D.y, PlayerList[selectedPlayer].LeftCalf3D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot3D.x, PlayerList[selectedPlayer].LeftFoot3D.y, PlayerList[selectedPlayer].LeftFoot3D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot3D.x, PlayerList[selectedPlayer].RightFoot3D.y, PlayerList[selectedPlayer].RightFoot3D.z).c_str());
			ImGui::Separator();
			ImGui::Text("Screen Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head2D.x, PlayerList[selectedPlayer].Head2D.y, PlayerList[selectedPlayer].Head2D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom2D.x, PlayerList[selectedPlayer].Bottom2D.y, PlayerList[selectedPlayer].Bottom2D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip2D.x, PlayerList[selectedPlayer].Hip2D.y, PlayerList[selectedPlayer].Hip2D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck2D.x, PlayerList[selectedPlayer].Neck2D.y, PlayerList[selectedPlayer].Neck2D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft2D.x, PlayerList[selectedPlayer].UpperArmLeft2D.y, PlayerList[selectedPlayer].UpperArmLeft2D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight2D.x, PlayerList[selectedPlayer].UpperArmRight2D.y, PlayerList[selectedPlayer].UpperArmRight2D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand2D.x, PlayerList[selectedPlayer].LeftHand2D.y, PlayerList[selectedPlayer].LeftHand2D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand2D.x, PlayerList[selectedPlayer].RightHand2D.y, PlayerList[selectedPlayer].RightHand2D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT2D.x, PlayerList[selectedPlayer].LeftHandT2D.y, PlayerList[selectedPlayer].LeftHandT2D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT2D.x, PlayerList[selectedPlayer].RightHandT2D.y, PlayerList[selectedPlayer].RightHandT2D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh2D.x, PlayerList[selectedPlayer].RightThigh2D.y, PlayerList[selectedPlayer].RightThigh2D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh2D.x, PlayerList[selectedPlayer].LeftThigh2D.y, PlayerList[selectedPlayer].LeftThigh2D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf2D.x, PlayerList[selectedPlayer].RightCalf2D.y, PlayerList[selectedPlayer].RightCalf2D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf2D.x, PlayerList[selectedPlayer].LeftCalf2D.y, PlayerList[selectedPlayer].LeftCalf2D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot2D.x, PlayerList[selectedPlayer].LeftFoot2D.y, PlayerList[selectedPlayer].LeftFoot2D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot2D.x, PlayerList[selectedPlayer].RightFoot2D.y, PlayerList[selectedPlayer].RightFoot2D.z).c_str());
			ImGui::Separator();
		}ImGui::End();
	}
}

void menu::AdvancedDebugWindow() {
	ImVec2 size = ImVec2(800, 500);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Advanced Debug"), NULL, ImGuiWindowFlags_NoResize) {

		int x = 0, y = 0;

		// first row
		x = 10; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Base -> 0x{:X}", point::Base).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("va_text -> 0x{:X}", point::va_text).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Uworld -> 0x{:X}", point::Uworld).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameInstance -> 0x{:X}", point::GameInstance).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PersistentLevel -> 0x{:X}", point::PersistentLevel).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameState -> 0x{:X}", point::GameState).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerController -> 0x{:X}", point::PlayerController).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPawn -> 0x{:X}", point::Player).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerCameraManager -> 0x{:X}", point::PlayerCameraManager).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerState -> 0x{:X}", point::PlayerState).c_str());

		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Players Rendered -> {:d}", info::render::playersRendered).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Players Looped -> {:d}", info::render::playersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Valid Players Looped -> {:d}", info::render::validPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Invalid Players Looped -> {:d}", info::render::invalidPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Valid Bots Looped -> {:d}", info::render::validBots).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Teammates Skipped -> {:d}", info::render::teammatesSkipped).c_str());

		// second row

		x = 310; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocationPointer -> 0x{:X}", point::LocationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("RotationPointer -> 0x{:X}", point::RotationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Location -> x: {:.2f} y: {:.2f} z: {:.2f}", mainCamera.Location.x, mainCamera.Location.y, mainCamera.Location.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Rotation -> x: {:.2f} y: {:.2f}", mainCamera.Rotation.x, mainCamera.Rotation.y).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera FieldOfView -> {:.2f} ", mainCamera.FieldOfView).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Location Under Reticle -> x: {:.2f} y: {:.2f} z: {:.2f}", mainCamera.LocationUnderReticle.x, mainCamera.LocationUnderReticle.y, mainCamera.LocationUnderReticle.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Seconds -> {:.2f}", point::Seconds).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Mouse Sens X -> {:.4f}", point::MouseSensX).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Mouse Sens Y -> {:.4f}", point::MouseSensY).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Local Team -> {:d}", local_player::localTeam).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("PlayerArray -> 0x{:X}", point::PlayerArray.data).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerArray Count -> {:d}", point::PlayerArray.count).c_str());

	} ImGui::End();


}