#pragma once

#include "global.h"
#include "utils.h"
#include "offsets.h"

bool update_va_text() {
	for (int i = 0, e = 0; i < INT_MAX && e < 3; i++) {
		point::va_text = point::Base + i * 0x1000;
		auto uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);
		auto level = mem.Read<uintptr_t>(uworld + offsets::PersistentLevel);
		if (uworld && level && mem.Read<uintptr_t>(level + offsets::OwningWorld) == uworld) {
			return true;
		}
		if (i > 1000) { i = 0; e++; };
	}
	return false;
}

void newInfo()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::GData)
		goto END;

	if (point::Uworld || point::GameInstance || point::GameState) {

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::va_text + offsets::Uworld, sizeof(uintptr_t));

		if (point::Uworld) {
			mem.SPrepare(mem.hS, point::Uworld + offsets::CameraLocationPointer, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::CameraRotationPointer, sizeof(uintptr_t));

			mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));
		}

		if (point::GameInstance) {
			mem.SPrepare(mem.hS, point::GameInstance + offsets::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayers) {
			mem.SPrepare(mem.hS, point::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayer) {
			mem.SPrepare(mem.hS, point::LocalPlayer + offsets::PlayerController, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::LocalPlayer + 0x378, sizeof(uintptr_t));
		}

		if (point::PlayerController) {
			mem.SPrepare(mem.hS, point::PlayerController + offsets::LocalPawn, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::PlayerController + offsets::Cameramanager, sizeof(uintptr_t));
		}

		if (point::Player) {
			mem.SPrepare(mem.hS, point::Player + offsets::PlayerState, sizeof(uintptr_t));
		}

		if (point::GameState) {
			mem.SPrepare(mem.hS, point::GameState + offsets::PlayerArray, sizeof(tarray));
		}

		if (point::PlayerState) {
			mem.SPrepare(mem.hS, point::PlayerState + offsets::TeamId, sizeof(uint32_t));
		}

		mem.ExecuteMemoryReads(mem.hS);

		point::Uworld = mem.SReads<uintptr_t>(mem.hS, point::va_text + offsets::Uworld);

		if (point::Uworld) {
			point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraLocationPointer);
			point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraRotationPointer);
			point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
			point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
			point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);
		}

		if (point::GameInstance) {
			point::LocalPlayers = mem.SReads<uintptr_t>(mem.hS, point::GameInstance + offsets::LocalPlayers);
		}

		if (point::LocalPlayers) {
			point::LocalPlayer = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayers);
		}

		if (point::LocalPlayer) {
			point::PlayerController = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + offsets::PlayerController);
			point::Settings = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + 0x378);
			if (point::Settings) {
				point::MouseSensX = mem.Read<float>(point::Settings + 0x664);
				point::MouseSensY = mem.Read<float>(point::Settings + 0x668);
			}
		}

		if (point::PlayerController) {
			point::Player = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::LocalPawn);
			point::PlayerCameraManager = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::Cameramanager);
		}

		if (point::Player) {
			point::PlayerState = mem.SReads<uintptr_t>(mem.hS, point::Player + offsets::PlayerState);
		}

		if (point::GameState) {
			point::PlayerArray = mem.SReads<tarray>(mem.hS, point::GameState + offsets::PlayerArray);
		}

		if (point::PlayerState) {
			local_player::localTeam = mem.SReads<uint32_t>(mem.hS, point::PlayerState + offsets::TeamId);
		}

	}
	else {

		point::Uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);

		if (!point::Uworld) goto END;

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraLocationPointer, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraRotationPointer, sizeof(uintptr_t));

		mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));

		mem.ExecuteMemoryReads(mem.hS);

		point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraLocationPointer);
		point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraRotationPointer);
		point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
		point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
		point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);

		if (!point::GameState) goto END;

		if (point::GameInstance) point::LocalPlayers = mem.Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);

		if (!point::LocalPlayers) goto END;

		point::LocalPlayer = mem.Read<uintptr_t>(point::LocalPlayers);

		if (!point::LocalPlayer) goto END;

		point::PlayerController = mem.Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);


		if (point::PlayerController) point::Player = mem.Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

		if (point::PlayerController) point::PlayerCameraManager = mem.Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

		if (point::Player) point::PlayerState = mem.Read<uintptr_t>(point::Player + offsets::PlayerState);

		point::PlayerArray = mem.Read<tarray>(point::GameState + offsets::PlayerArray);

		if (point::PlayerState) local_player::localTeam = mem.Read<uint32_t>(point::PlayerState + offsets::TeamId);
	}

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::GDataData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000)
		std::cout << hue::yellow << "(/) " << hue::white << "Warning GData Update took " << elapsed << " ms" << std::endl;

}

#include <mutex>
std::mutex listLock;

void updatePlayerList()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::PlayerList) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}

	// check if we can update players
	if (!point::Uworld && !point::GameState && !point::PlayerArray.data && point::PlayerArray.count > 0 && point::PlayerArray.count < 150) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}

	// we will save the data used for this update (so they dont change while we are using them)
	tarray playerArray = point::PlayerArray;

	std::unique_ptr<uintptr_t[]> players(new uintptr_t[playerArray.count]); // this is an array of player states

	// read the whole player list
	mem.ReadA(playerArray.data, players.get(), playerArray.count * sizeof(uintptr_t));

	// clean cache off of people that dont exist anymore
	listLock.lock();
	auto it = mainPlayerList.begin();
	while (it != mainPlayerList.end()) {
		//PlayerCache cachedPlayer = it->second;
		bool found = false;
		for (int i = 0; i < playerArray.count; i++) {
			if (players[i] == it->first) {
				found = true;
				break;
			}
		}
		if (!found) {
			it = mainPlayerList.erase(it);
		}
		else {
			++it;
		}
	}
	listLock.unlock();

	// add people that havent been cached yet
	std::vector<PlayerCache> playersToAdd;
	for (int i = 0; i < playerArray.count; i++) {
		if (!players[i])
			continue;
		auto it = mainPlayerList.find(players[i]);
		if (it == mainPlayerList.end()) {
			PlayerCache newPlayer;
			newPlayer.PlayerState = players[i];
			playersToAdd.push_back(newPlayer);
		}
	}

	// are there new players to add?
	if (playersToAdd.size() > 0) {
		// start looping over the new players to prepare data
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId, sizeof(uint32_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot, sizeof(BYTE));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// read the prepared data
		for (int i = 0; i < playersToAdd.size(); i++) {
			playersToAdd[i].Pawn = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate);
			playersToAdd[i].TeamId = mem.SReads<uint32_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId);
			playersToAdd[i].isBot = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot) & 0x08;
		}

		// now that we have everything we just add them to the cache
		listLock.lock();
		for (int i = 0; i < playersToAdd.size(); i++) {
			mainPlayerList[playersToAdd[i].PlayerState] = playersToAdd[i];
		}
		listLock.unlock();
	}

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayerListData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000)
		std::cout << hue::yellow << "(/) " << hue::white << "Warning Players Update took " << elapsed << " ms" << std::endl;
}

#include <execution>

void MainUpdate()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::Players) {
		goto END;
	}

	listLock.lock();

	secondPlayerList = mainPlayerList;

	mem.SClear(mem.hS4);

	// camera
	FNRot rotation;
	mem.SPrepareEx(mem.hS4, point::RotationPointer, sizeof(FNRot), &rotation);
	mem.SPrepareEx(mem.hS4, point::LocationPointer, sizeof(Vector3), &mainCamera.Location);
	mem.SPrepareEx(mem.hS4, point::LocalPlayer + offsets::FieldOfView, sizeof(float), &mainCamera.FieldOfView);

	// while we are at it update seconds for vischeck
	mem.SPrepareEx(mem.hS4, point::Uworld + offsets::Seconds, sizeof(double), &point::Seconds);

	// and location under reticle
	if (point::PlayerController)
		mem.SPrepareEx(mem.hS4, point::PlayerController + offsets::LocationUnderReticle, sizeof(Vector3), &mainCamera.LocationUnderReticle);

	// prepare all the reads in parallel
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {

		if (!it.second.Pawn || point::Player && it.second.TeamId == local_player::localTeam) {
			it.second.ignore = true;
		}
		else {
			if (it.second.BoneArray) {
				mem.SPrepareEx(mem.hS4, it.second.BoneArray, (82 * 0x60) + sizeof(FTransform), it.second.Bones); // 82
			}

			if (it.second.Mesh) {
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::BoneArray, sizeof(it.second.BoneArrays), it.second.BoneArrays);
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::ComponentToWorld, sizeof(FTransform), &it.second.component_to_world);
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::LastRenderTime, sizeof(float), &it.second.last_render);
				mem.SPrepare(mem.hS4, it.second.Mesh + offsets::LastRenderTime, sizeof(float));
			}

			if (it.second.Pawn && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastMesh).count() > 500) {
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::Mesh, sizeof(uintptr_t), &it.second.Mesh);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::RootComponent, sizeof(uintptr_t), &it.second.RootComponent);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::isDying, sizeof(BYTE), &it.second.isDying);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::IsDBNO, sizeof(BYTE), &it.second.isDBNO);
				it.second.lastMesh = std::chrono::system_clock::now();
			}

			if (it.second.RootComponent && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastVelocity).count() > 100) {
				mem.SPrepareEx(mem.hS4, it.second.RootComponent + offsets::Velocity, sizeof(Vector3), &it.second.Velocity);
				it.second.lastVelocity = std::chrono::system_clock::now();
			}

			it.second.ignore = false;
		}

		if (it.second.PlayerState && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastPawn).count() > it.second.Pawn ? 500 : 2500) {
			mem.SPrepareEx(mem.hS4, it.second.PlayerState + offsets::PawnPrivate, sizeof(uintptr_t), &it.second.Pawn);
			it.second.lastPawn = std::chrono::system_clock::now();
		}

		if (it.second.PlayerState && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastTeamId).count() > 2500) {
			mem.SPrepareEx(mem.hS4, it.second.PlayerState + offsets::TeamId, sizeof(uint32_t), &it.second.TeamId);
			it.second.lastTeamId = std::chrono::system_clock::now();
		}

		});

	// EXECUTE AHAHAHAHHAHAH
	mem.ExecuteMemoryReads(mem.hS4);

	// camera rotation
	mainCamera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	mainCamera.Rotation.y = atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI);

	// all calcs in parallel ?
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {

		if (!it.second.ignore)
		{
			it.second.lastUpdate = chrono::system_clock::now();

			if (it.second.BoneArray) {

				// get bones world position
				it.second.Head3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::HeadBone)].Bone, it.second.component_to_world);
				it.second.Bottom3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::BottomBone)].Bone, it.second.component_to_world);

				it.second.Hip3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::HipBone)].Bone, it.second.component_to_world);
				it.second.Neck3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::NeckBone)].Bone, it.second.component_to_world);
				it.second.UpperArmLeft3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::UpperArmLeftBone)].Bone, it.second.component_to_world);
				it.second.UpperArmRight3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::UpperArmRightBone)].Bone, it.second.component_to_world);
				it.second.LeftHand3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftHandBone)].Bone, it.second.component_to_world);
				it.second.RightHand3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightHandBone)].Bone, it.second.component_to_world);
				it.second.LeftHandT3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftHandTBone)].Bone, it.second.component_to_world);
				it.second.RightHandT3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightHandTBone)].Bone, it.second.component_to_world);
				it.second.RightThigh3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightThighBone)].Bone, it.second.component_to_world);
				it.second.LeftThigh3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftThighBone)].Bone, it.second.component_to_world);
				it.second.RightCalf3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightCalfBone)].Bone, it.second.component_to_world);
				it.second.LeftCalf3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftCalfBone)].Bone, it.second.component_to_world);
				it.second.LeftFoot3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftFootBone)].Bone, it.second.component_to_world);
				it.second.RightFoot3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightFootBone)].Bone, it.second.component_to_world);


				// and do world to screen
				it.second.Head2D = w2s(it.second.Head3D);

				it.second.Top2D = w2s(Vector3(it.second.Head3D.x, it.second.Head3D.y, it.second.Head3D.z + 15.f));
				it.second.Bottom2D = w2s(it.second.Bottom3D);

				it.second.Hip2D = w2s(it.second.Hip3D);
				it.second.Neck2D = w2s(it.second.Neck3D);
				it.second.UpperArmLeft2D = w2s(it.second.UpperArmLeft3D);
				it.second.UpperArmRight2D = w2s(it.second.UpperArmRight3D);
				it.second.LeftHand2D = w2s(it.second.LeftHand3D);
				it.second.RightHand2D = w2s(it.second.RightHand3D);
				it.second.LeftHandT2D = w2s(it.second.LeftHandT3D);
				it.second.RightHandT2D = w2s(it.second.RightHandT3D);
				it.second.RightThigh2D = w2s(it.second.RightThigh3D);
				it.second.LeftThigh2D = w2s(it.second.LeftThigh3D);
				it.second.RightCalf2D = w2s(it.second.RightCalf3D);
				it.second.LeftCalf2D = w2s(it.second.LeftCalf3D);
				it.second.LeftFoot2D = w2s(it.second.LeftFoot3D);
				it.second.RightFoot2D = w2s(it.second.RightFoot3D);
			}

			it.second.bisDying = it.second.isDying & 0x20;
			it.second.bisDBNO = (it.second.isDBNO >> 6) & 1;

			it.second.BoneArray = it.second.BoneArrays[0].data;
			if (!it.second.BoneArray) it.second.BoneArray = it.second.BoneArrays[1].data;

		}

		});

	mainPlayerList = secondPlayerList;
	listLock.unlock();

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayersData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000000)
		std::cout << hue::yellow << "(/) " << hue::white << "Warning Main Update took " << elapsed / 1000 << " ms" << std::endl;
}

void weaponUpdate()
{
	// features that need the weapon updated
	if (!settings::config::Aimbot && !settings::config::TriggerBot)
		return;

	mem.SClear(mem.hS5);

	if (point::Player)
		mem.SPrepare(mem.hS5, point::Player + offsets::CurrentWeapon, sizeof(uintptr_t));

	if (point::CurrentWeapon) {
		mem.SPrepare(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileSpeed, sizeof(float));
		mem.SPrepare(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileGravity, sizeof(float));
	}

	mem.ExecuteMemoryReads(mem.hS5);

	float projectileSpeed = 0;

	if (point::Player)
		point::CurrentWeapon = mem.SReads<uintptr_t>(mem.hS5, point::Player + offsets::CurrentWeapon);

	if (point::Player && point::CurrentWeapon) {
		point::ProjectileSpeed = mem.SReads<float>(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileSpeed) / settings::config::PredictionMultiplier;
		point::ProjectileGravity = mem.SReads<float>(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileGravity);
	}
	else {
		point::ProjectileSpeed = 0;
		point::ProjectileGravity = 0;
	}
}


void healthChecks() {

	// future

}
