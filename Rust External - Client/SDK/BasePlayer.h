#pragma once

enum BoneList : int
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,
	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement
};

typedef struct Str
{
	char stub[0x10];
	int len;
	wchar_t str[1];
} *str;


class BasePlayer
{
public:

	/*Player Info*/

	UINT_PTR GetName() {
		auto player_name = safe_read((uintptr_t)this + oPlayerName, UINT_PTR);
		return player_name;
	}

	UINT64 GetSteamID()
	{
		return safe_read((const uintptr_t)(const uintptr_t)this + oSteamID, UINT64);
	}

	float GetHealth()
	{
		return safe_read((const uintptr_t)(const uintptr_t)this + oPlayerHealth, float);
	}

	bool HasFlags(int Flg)
	{
		return safe_read((const uintptr_t)(const uintptr_t)this + oPlayerFlags, int) & Flg;
	}

	bool IsDead()
	{
		if (!(const uintptr_t)this) return true;
		return safe_read((const uintptr_t)(const uintptr_t)this + oLifestate, bool);
	}

	bool IsValid(bool LPlayer = false)
	{
		//std::cout << "ISvalid " <<HasFlags(16) << "  " << IsDead() << " " << GetHealth() << std::endl;
		if (!this) return false;
		float health = GetHealth();
		UINT64 steam = GetSteamID();
		//std::cout << health << " " << steam;
		if (health > 150.f || health < 0.8f) return false;


		int nums = 0;
		do {
			nums++;
			steam /= 10;
		} while (steam > 0);
		//std::cout << " " << nums << std::endl;
		if (nums != 7 /*bot*/ && nums != 17 /*player*/) return false;
		return (((LPlayer || Value::bools::Visuals::ESP::IgnoreSleeper) ? !HasFlags(16) : true) && !IsDead());
	}

	/*Vector*/

	Vector2 GetVA()
	{
		UINT64 Input = safe_read((const uintptr_t)this + oPlayerInput, UINT64);
		return safe_read(Input + oBodyAngles, Vector2);
	}

	Vector3 GetVelocity()
	{
		UINT64 PlayerModel = safe_read((const uintptr_t)(const uintptr_t)this + oPlayerModel, UINT64);
		return safe_read(PlayerModel + 0x1FC, Vector3); // 	private Vector3 newVelocity;
		//обновил
	}

	Vector3 GetBoneByID2(BoneList BoneID) {
		return GetBone(GetTransform(BoneID));
	}

	Vector3 GetBoneByID()
	{
		return GetBone(GetTransform(BoneList(neck))); //////////////////////////////////////////////////////////////////
	}

	bool IsVisible() {
		if (Value::bools::Aim::VisibleCheck) {
			UINT64 PlayerModel = safe_read((const uintptr_t)(const uintptr_t)this + oPlayerModel, UINT64);
			return safe_read(PlayerModel + 0x248, bool); // internal bool visible; WORK
		}
		else return true;
	}

	/*Fucktion*/

	void SetVA(const Vector2& VA)
	{
		UINT64 Input = safe_read((const uintptr_t)this + oPlayerInput, UINT64);
		safe_write(Input + oBodyAngles, VA, Vector2);
	}

	void FakeAdmin()
	{
		int Flags = safe_read((const uintptr_t)this + oPlayerFlags, int);
		safe_write((const uintptr_t)this + oPlayerFlags, (Flags |= 4), int);
	}

	void InfinityJump()
	{
		auto PlayerWalkMovement = safe_read((const uintptr_t)this + oPlayerMovement, uintptr_t);
		safe_write(PlayerWalkMovement + 0xC0, 0, float); //private float jumpTime;
		safe_write(PlayerWalkMovement + 0xC4, 0, float); //private float landTime;
		safe_write(PlayerWalkMovement + 0xBC, 10000, float); //private float groundTime;
	}

	void GravituMod()
	{
		UINT64 Movement = safe_read((const uintptr_t)this + oPlayerMovement, UINT64);
		float Gravity = 2.5f;
		if (GetKeyState(VK_MBUTTON))
		{
			safe_write(Movement + oGravityMultiplier, Gravity / Value::floats::Player::Gravity, float);
		}
		else
		{
			safe_write(Movement + oGravityMultiplier, Gravity, float);
		}
	}

	void SetFov()
	{
		auto Class = safe_read(gBase + oConVar, DWORD64);
		auto Field = safe_read(Class + 0xB8, DWORD64);
		safe_write(Field + 0x18, Value::floats::Player::CustomFov, float);
	}

	void AntiAim() {
		auto PlayerInput = safe_read((uintptr_t)this + 0x4D0, DWORD64); //public PlayerInput input;
		auto State = safe_read(PlayerInput + 0x20, DWORD64); //public InputState state;
		auto Current = safe_read(State + 0x10, DWORD64); //public InputMessage current;
		safe_write(Current + 0x18, Vector3(rand() % 999 + -999, rand() % 999 + -999, rand() % 999 + -999), Vector3); //Body->PlayerInput
		safe_write(Current + 0x58, Vector3(rand() % 999 + -999, rand() % 999 + -999, rand() % 999 + -999), Vector3); //head->PlayerInput
		safe_write(Current + 0x70, Vector3(rand() % 999 + -999, rand() % 999 + -999, rand() % 999 + -999), Vector3); //public Vector3 aimAngles;
	}

	void SpiderMan()
	{
		UINT64 Movement = safe_read((const uintptr_t)this + oPlayerMovement, UINT64);
		//safe_write(Movement + 0xA4, 0.f, float);
		safe_write(Movement + oGroundAngleNew, 0.f, float);
		//safe_write(Movement + 0x124, 1, bool);
	}

	/*Weapon Info*/
	BaseProjectile* GetWeaponInfo(int Id)
	{
		UINT64 Inventory = safe_read((const uintptr_t)this + oPlayerInventory, UINT64);
		UINT64 Belt = safe_read(Inventory + 0x28, UINT64);
		UINT64 ItemList = safe_read(Belt + 0x38, UINT64);
		UINT64 Items = safe_read(ItemList + 0x10, UINT64);
		//	std::cout << "item " << safe_read(Items + 0x20 + (Id * 0x8), UINT64) << std::endl;
		return (BaseProjectile*)safe_read(Items + 0x20 + (Id * 0x8), UINT64);
	}

	BaseProjectile* GetActiveWeapon()
	{
		int ActUID = safe_read((const uintptr_t)this + oActiveUID, int);
		if (!ActUID) return nullptr;
		BaseProjectile* ActiveWeapon;
		for (int i = 0; i < 6; i++)
			if (ActUID == (ActiveWeapon = GetWeaponInfo(i))->GetUID()) {
				//std::cout << "uid " << ActUID << std::endl;
				return ActiveWeapon;
			}
		return nullptr;
	}



	/*Local-Player*/
	bool IsMenu()
	{
		if (!(const uintptr_t)this) return true;
		UINT64 Input = safe_read((const uintptr_t)(const uintptr_t)this + oPlayerInput, UINT64);
		return !(safe_read(Input + 0x94, bool));// private bool hasKeyFocus;
	}

	int GetTeamSize()
	{
		UINT64 ClientTeam = safe_read((const uintptr_t)(const uintptr_t)this + oClientTeam, UINT64); // valid
		UINT64 members = safe_read(ClientTeam + 0x30, UINT64);
		return safe_read(members + 0x18, UINT64);
	}

	UINT64 IsTeamMate(int Id)
	{
		UINT64 ClientTeam = safe_read((const uintptr_t)(const uintptr_t)this + oClientTeam, UINT64);
		UINT64 members = safe_read(ClientTeam + 0x30, UINT64);
		UINT64 List = safe_read(members + 0x10, UINT64);
		UINT64 player = safe_read(List + 0x20 + (Id * 0x8), UINT64);
		return safe_read(player + 0x20, UINT64);
	}

	std::string GetNamecChars() {
		return readstring(this->GetName());
	}

	std::string GetActiveWeaponcChars() {
		BaseProjectile* ActWeapon = this->GetActiveWeapon();
		string ActiveWeaponName;
		if (!ActWeapon)
		{
			ActiveWeaponName = "Empty";

		}
		else
		{
			ActiveWeaponName = readstring(ActWeapon->GetName());
		}
		return ActiveWeaponName;
	}

private:

	struct TransformAccessReadOnly
	{
		uint64_t pTransformData;
	};

	struct TransformData
	{
		uint64_t pTransformArray;
		uint64_t pTransformIndices;
	};

	struct Matrix34
	{
		Vector4 vec0;
		Vector4 vec1;
		Vector4 vec2;
	};

	Vector3 GetBone(ULONG_PTR pTransform)
	{
		__m128 result;

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		TransformAccessReadOnly pTransformAccessReadOnly = safe_read(pTransform + 0x38, TransformAccessReadOnly);
		unsigned int index = safe_read(pTransform + 0x40, unsigned int);
		TransformData transformData = safe_read(pTransformAccessReadOnly.pTransformData + 0x18, TransformData);

		if (transformData.pTransformArray && transformData.pTransformIndices)
		{
			result = safe_read(transformData.pTransformArray + 0x30 * index, __m128);
			int transformIndex = safe_read(transformData.pTransformIndices + 0x4 * index, int);
			int pSafe = 0;
			while (transformIndex >= 0 && pSafe++ < 200)
			{
				Matrix34 matrix34 = safe_read(transformData.pTransformArray + 0x30 * transformIndex, Matrix34);

				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));	// xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));	// yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));	// zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));	// wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));	// zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));	// yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

				result = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
							tmp7)), *(__m128*)(&matrix34.vec0));

				transformIndex = safe_read(transformData.pTransformIndices + 0x4 * transformIndex, int);
			}
		}

		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}

	DWORD64 GetTransform(int bone)
	{
		DWORD64 model = safe_read((const uintptr_t)this + 0x120, DWORD64);// public Model model;_public class BaseEntity : BaseNetworkable, IProvider, ILerpTarget, IPrefabPreProcess // TypeDefIndex: 8714
		DWORD64 boneTransforms = safe_read(model + 0x48, DWORD64);//public Transform[] boneTransforms;
		DWORD64 BoneValue = safe_read((boneTransforms + (0x20 + (bone * 0x8))), DWORD64);
		return safe_read(BoneValue + 0x10, DWORD64);
	}
};


class LPlayerBase
{
public:
	BasePlayer* BasePlayer = nullptr;
	Matrix4x4* pViewMatrix = nullptr;
	bool WorldToScreen(const Vector3& EntityPos, Vector2& ScreenPos)
	{
		if (!pViewMatrix) return false;
		Vector3 TransVec = Vector3(safe_read((ULONG64)&pViewMatrix->_14, float), safe_read((ULONG64)&pViewMatrix->_24, float), safe_read((ULONG64)&pViewMatrix->_34, float));
		Vector3 RightVec = Vector3(safe_read((ULONG64)&pViewMatrix->_11, float), safe_read((ULONG64)&pViewMatrix->_21, float), safe_read((ULONG64)&pViewMatrix->_31, float));
		Vector3 UpVec = Vector3(safe_read((ULONG64)&pViewMatrix->_12, float), safe_read((ULONG64)&pViewMatrix->_22, float), safe_read((ULONG64)&pViewMatrix->_32, float));
		float w = Math::Dot(TransVec, EntityPos) + safe_read((ULONG64)&pViewMatrix->_44, float);
		if (w < 0.098f) return false;
		float y = Math::Dot(UpVec, EntityPos) + safe_read((ULONG64)&pViewMatrix->_42, float);
		float x = Math::Dot(RightVec, EntityPos) + safe_read((ULONG64)&pViewMatrix->_41, float);
		ScreenPos = Vector2((Value::floats::Screen::W / 2) * (1.f + x / w), (Value::floats::Screen::H / 2) * (1.f - y / w));
		return true;
	}
};

LPlayerBase LocalPlayer;