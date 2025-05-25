#pragma once

#include <cstdint>
#include <wtypes.h>
#include <corecrt_math.h>
#include <chrono>

#define M_PI 3.14159265358979323846

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	double x;
	double y;
};
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	static double GetDistance(Vector3 point1, Vector3 point2)
	{
		Vector3 heading = point2 - point1;
		double distanceSquared;
		double distance;

		distanceSquared = heading.x * heading.x + heading.y * heading.y + heading.z * heading.z;
		distance = sqrt(distanceSquared);

		return distance;
	}

	void addScaled(const Vector3& v, float scale)
	{
		x += v.x * scale;
		y += v.y * scale;
		z += v.z * scale;
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(double number) const {
		return Vector3(x * number, y * number, z * number);
	}

	Vector3 operator/(double number) const {
		return Vector3(x / number, y / number, z / number);
	}

	bool operator==(Vector3 v) const {
		return x == v.x && y == v.y && z == v.z;
	}
};
struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};

typedef struct FMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad01[4];
	Vector3 scale;
	FMATRIX ToMatrixWithScale()
	{
		FMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
class FRotator
{
public:
	FRotator() : Pitch(0.f), Yaw(0.f), Roll(0.f)
	{

	}

	FRotator(double _Pitch, double _Yaw, double _Roll) : Pitch(_Pitch), Yaw(_Yaw), Roll(_Roll)
	{

	}
	~FRotator()
	{

	}

	double Pitch;
	double Yaw;
	double Roll;
	inline FRotator get() {
		return FRotator(Pitch, Yaw, Roll);
	}
	inline void set(double _Pitch, double _Yaw, double _Roll) {
		Pitch = _Pitch;
		Yaw = _Yaw;
		Roll = _Roll;
	}

	inline FRotator Clamp() {
		FRotator result = get();
		if (result.Pitch > 180)
			result.Pitch -= 360;
		else if (result.Pitch < -180)
			result.Pitch += 360;
		if (result.Yaw > 180)
			result.Yaw -= 360;
		else if (result.Yaw < -180)
			result.Yaw += 360;
		if (result.Pitch < -89)
			result.Pitch = -89;
		if (result.Pitch > 89)
			result.Pitch = 89;
		while (result.Yaw < -180.0f)
			result.Yaw += 360.0f;
		while (result.Yaw > 180.0f)
			result.Yaw -= 360.0f;

		result.Roll = 0;
		return result;

	}
	double Length() {
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	FRotator operator+(FRotator angB) { return FRotator(Pitch + angB.Pitch, Yaw + angB.Yaw, Roll + angB.Roll); }
	FRotator operator-(FRotator angB) { return FRotator(Pitch - angB.Pitch, Yaw - angB.Yaw, Roll - angB.Roll); }
	FRotator operator/(float flNum) { return FRotator(Pitch / flNum, Yaw / flNum, Roll / flNum); }
	FRotator operator*(float flNum) { return FRotator(Pitch * flNum, Yaw * flNum, Roll * flNum); }
	bool operator==(FRotator angB) { return Pitch == angB.Pitch && Yaw == angB.Yaw && Yaw == angB.Yaw; }
	bool operator!=(FRotator angB) { return Pitch != angB.Pitch || Yaw != angB.Yaw || Yaw != angB.Yaw; }

};

struct Camera
{
	Vector3 Location{};
	Vector3 Rotation{};
	float FieldOfView = 0;
	Vector3 LocationUnderReticle{};
};

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

struct tarray
{
	uintptr_t data;
	uint32_t count;
	uint32_t max;
};

enum class BoneID : int {
	HeadBone = 68,
	BottomBone = 0,
	NeckBone = 66,
	HipBone = 2,
	UpperArmLeftBone = 9,
	UpperArmRightBone = 38,
	LeftHandBone = 10,
	RightHandBone = 39,
	LeftHandTBone = 11,
	RightHandTBone = 40,
	RightThighBone = 78,
	LeftThighBone = 71,
	RightCalfBone = 79,
	LeftCalfBone = 72,
	LeftFootBone = 75,
	RightFootBone = 82
};

struct PlayerCache {
	bool ignore = false;

	chrono::system_clock::time_point lastUpdate = std::chrono::system_clock::now();

	uintptr_t PlayerState = 0;
	uintptr_t Pawn = 0;
	chrono::system_clock::time_point lastPawn = std::chrono::system_clock::now();
	uintptr_t Mesh = 0;
	uintptr_t RootComponent = 0;
	chrono::system_clock::time_point lastMesh = std::chrono::system_clock::now(); // root component too
	uintptr_t BoneArray = 0;
	tarray BoneArrays[2]{};
	chrono::system_clock::time_point lastBoneArray = std::chrono::system_clock::now();
	Vector3 Velocity{};
	chrono::system_clock::time_point lastVelocity = std::chrono::system_clock::now();
	uint32_t TeamId = 0;
	chrono::system_clock::time_point lastTeamId = std::chrono::system_clock::now();
	float last_render = 0;

	BYTE isDying = 0;
	BYTE isDBNO = 0;
	bool isBot = 0;

	bool bisDying = false;
	bool bisDBNO = false;

	struct {
		FTransform Bone{};
		char pad[8]{};
	}Bones[83]{};

	FTransform component_to_world{};

	// world
	Vector3 Head3D{};
	Vector3 Bottom3D{};

	Vector3 Hip3D{};
	Vector3 Neck3D{};
	Vector3 UpperArmLeft3D{};
	Vector3 UpperArmRight3D{};
	Vector3 LeftHand3D{};
	Vector3 RightHand3D{};
	Vector3 LeftHandT3D{};
	Vector3 RightHandT3D{};
	Vector3 RightThigh3D{};
	Vector3 LeftThigh3D{};
	Vector3 RightCalf3D{};
	Vector3 LeftCalf3D{};
	Vector3 LeftFoot3D{};
	Vector3 RightFoot3D{};

	// screen
	Vector3 Head2D{};

	Vector3 Top2D{};
	Vector3 Bottom2D{};

	Vector3 Hip2D{};
	Vector3 Neck2D{};
	Vector3 UpperArmLeft2D{};
	Vector3 UpperArmRight2D{};
	Vector3 LeftHand2D	{};
	Vector3 RightHand2D{};
	Vector3 LeftHandT2D{};
	Vector3 RightHandT2D{};
	Vector3 RightThigh2D{};
	Vector3 LeftThigh2D{};
	Vector3 RightCalf2D{};
	Vector3 LeftCalf2D{};
	Vector3 LeftFoot2D{};
	Vector3 RightFoot2D{};

	bool UsedByAim = false;
	Vector3 Aim{};
};

inline bool isPlayerValid(PlayerCache player) {
	return player.Pawn && player.BoneArray;
}

struct Rotation {
	float yaw;   // Rotation around Y-axis
	float pitch; // Rotation around X-axis
};