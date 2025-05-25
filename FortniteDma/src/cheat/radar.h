#pragma once

namespace radar {
	static Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
	{
		float r_1, r_2;
		float x_1, y_1;

		r_1 = -(EntityPos.y - LocalPlayerPos.y);
		r_2 = (EntityPos.x - LocalPlayerPos.x);
		float Yaw = angle - 90;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

		*viewCheck = y_1 < 0;

		x_1 *= -zoom;
		y_1 *= zoom;

		int sizX = sizeX / 2;
		int sizY = sizeY / 2;

		x_1 += sizX;
		y_1 += sizY;

		if (x_1 < 5)
			x_1 = 5;

		if (x_1 > sizeX - 5)
			x_1 = sizeX - 5;

		if (y_1 < 5)
			y_1 = 5;

		if (y_1 > sizeY - 5)
			y_1 = sizeY - 5;


		x_1 += posX;
		y_1 += posY;


		return Vector3(x_1, y_1, 0);
	}

	void DrawPoint(ImDrawList* drawlist, Vector3 position, int distance, Vector3 localPosition, int scale, float zoom, double localYaw, ImVec2 pos, ImVec2 siz, ImColor color)
	{
		bool out = false;
		bool ck = false;
		Vector3 single = RotatePoint(position, localPosition, pos.x, pos.y, siz.x, siz.y, localYaw, zoom, &ck);
		if (distance >= 0.f && distance < scale)
		{
			drawlist->AddCircleFilled(ImVec2(single.x, single.y), 5, color);

		}
	}

	void Render()
	{

		if (!settings::config::Radar)
			return;

		std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;

		ImGui::Begin("Radar", nullptr, ImGuiWindowFlags_NoTitleBar);

		ImVec2 RealSiz = ImGui::GetWindowSize();

		// minimum size
		{
			float minSize = 150;
			if (RealSiz.x < minSize) {
				ImGui::SetWindowSize(ImVec2(minSize, RealSiz.y));
			}
			if (RealSiz.y < minSize) {
				ImGui::SetWindowSize(ImVec2(RealSiz.x, minSize));
			}
		}

		ImVec2 siz = ImVec2(RealSiz.x - 10, RealSiz.y - 10);

		ImVec2 pos = ImGui::GetWindowPos();

		pos.x += 5;
		pos.y += 5;

		ImVec2 RadarCenter(pos.x + siz.x / 2, pos.y + siz.y / 2);

		// Access the window draw list
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Draw outline box
		ImU32 outlineColor = IM_COL32(75, 0, 95, 255);
		drawList->AddRect(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), outlineColor, 10.f, NULL, 20.f);

		// Draw filled box
		ImU32 fillColor = IM_COL32(25, 25, 25, 255);
		drawList->AddRectFilled(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), fillColor);

		ImU32 lineColor = IM_COL32(75, 75, 75, 255);
		drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x * 0.10), pos.y), lineColor, 4.f);
		drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x * 0.90), pos.y), lineColor, 4.f);
		drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), lineColor, 5.f);

		// draw the points in the radar
		for (auto it : PlayerList) {
			PlayerCache player = it.second;

			if (!isPlayerValid(player))
				continue;

			if (player.PlayerState == point::PlayerState) {
				continue;
			}

			if (point::Player && player.TeamId == local_player::localTeam) {
				continue;
			}

			if (player.bisDying)
				continue;

			// should be in the updates
			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			ImColor color = ImColor(255, 0, 0, 255);

			// the order matters

			// red not visible
			if (IsVis)
				color = ImColor(0, 255, 0, 255);

			// orange knocked players
			if (player.bisDBNO)
				color = ImColor(255, 100, 0, 255);

			// blue bots
			if (player.isBot)
				color = ImColor(0, 0, 255, 255);

			DrawPoint(drawList, player.Head3D, mainCamera.Location.Distance(player.Head3D), mainCamera.Location, settings::config::RadarScale, settings::config::RadarZoom, mainCamera.Rotation.y, pos, siz, color);

		}

		ImGui::End();
	}
}