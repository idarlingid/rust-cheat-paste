#pragma once
DWORD64 automatic[]
{
	1545779598, // AK - 47;
	-1758372725, // Томсон
	1796682209, // SMG
	-1812555177,// LR - 300
	1318558775, // MP5
	-2069578888, // M249
};

DWORD64 sniper[]
{
	-778367295, // L96
	1588298435, // Болт
};

DWORD64 semiautomatic[]{
	-904863145, // Берданка 
	818877484, // Пэшка
	-852563019, // берета
	1373971859, // Питон
	649912614, // револьвер
	28201841, // M39
	1953903201, // Гвоздомёт
	-1123473824, // Гранатомёт
};

//DWORD64 meele[]{ 
//	-1978999529, 1602646136, -194509282, -1966748496, 1326180354, 1540934679
//};
//nopatch 884424049(compound), 
//DWORD64 bow = 1802482880;

#define oGameObjectManager 0x17C1F18 //0x17C1F18 0x17D69F8
#define oBaseNetworkable   50746992 //0x29BF3E0 BaseNetworkable_c* 3065630
#define oConVar 50742120 //ConVar_Graphics_c*


//class BasePlayer
#define oPlayerFlags 0x638 // public BasePlayer.PlayerFlags playerFlags;
#define oPlayerName 0x698 // protected string _displayName;
#define oPlayerHealth 0x214 // private float _health;
#define olastSentTickTime 0x60C // не требо private float lastSentTickTime;
#define oClientTeam 0x580 // public PlayerTeam clientTeam;
#define oLifestate 0x20C // public BaseCombatEntity.LifeState lifestate;
#define oSteamID 0x680 // public ulong userID;
#define oPlayerInventory 0x648 // public PlayerInventory inventory;
#define oActiveUID 0x5B0// private uint clActiveItem;
#define oFrozen 0x4B8// public bool Frozen;
#define oWaterBonus 0x700// public float clothingWaterSpeedBonus;
#define oNoBlockAiming 0x6F8 // public bool clothingBlocksAiming;
#define oSpeedReduction 0x6FC// public float clothingMoveSpeedReduction;
#define oClothingAccuracyBonus 0x704 //	public float clothingAccuracyBonus;


//EntityRef
#define oHeld 0x98 //private EntityRef heldEntity ..private EntityRef heldEntity; // 0x98 //private EntityRef heldEntity
#define oStancePenalty 0x30C //private float stancePenalty;
#define	oAimconePenalty	0x310 //private float aimconePenalty;
#define	oHipAimCone 0x2DC //public float hipAimCone;
#define	oAimCone 0x2D8 //public float aimCone;			(class BaseProjectile)
#define	oAimconePenaltyPerShot 0x2E0 //public float aimconePenaltyPerShot;
#define oSuccessFraction 0x350 // public float successFraction;
#define oAttackRadius 0x284 //public float attackRadius;
#define oEffectiveRange 0x1EC //public float effectiveRange;
#define oIsAutomatic 0x288 //public bool isAutomatic;
#define oMaxDistance 0x280 //public float maxDistance;			(class BaseMelee)
#define oRepeatDelay 0x1E4 //public float repeatDelay;
#define odisplayname 0x28 //public Translate.Phrase displayName;

//BaseProjectile
#define oAuto 0x278 //public bool automatic;
#define oFastReload 0x2B0 // bool fractionalReload
#define oDistance 0x270 // public float distanceScale
#define oprojectileVelocityScale 0x274 //public float projectileVelocityScale
#define oShortname 0x20// public string shortname;

//BaseMovement
#define oPlayerMovement 0x4D8 // public BaseMovement movement;
#define oGravityMultiplier 0x74 // public float gravityMultiplier;

#define oGroundAngleNew 0xB8 // private float groundAngleNew;

//RecoilProperties
#define oRecoilProperties 0x2C8 //public RecoilProperties recoil;
#define oRecoilMinYaw 0x18 //public float recoilYawMin;
#define oRecoilMaxYaw 0x1C //public float recoilYawMax;
#define oRecoilMinPitch 0x20 //public float recoilPitchMin;
#define oRecoilMaxPitch 0x24 //public float recoilPitchMax;


//PlayerModel
#define oPlayerModel 0x4B0 // public PlayerModel playerModel;
#define oVisible 0x248 // internal bool visible;

//PlayerInput
#define oPlayerInput 0x4D0// public PlayerInput input;
#define oBodyAngles 0x3C // private Vector3 bodyAngles;
#define oRecoilAngles 0x64 // public Vector3 recoilAngles;
#define oHasKeyFocus 0x94 // private bool hasKeyFocus;


#define oArrowBack 0x354 // private float arrowBack;
#define oTriggerReady 0x350 // protected bool attackReady;
#define oItemDefinition 0x20 // public ItemDefinition info;
#define oItemid 0x18 // public int itemid;

////class BasePlayer
//#define oPlayerFlags 0x5F8 // public BasePlayer.PlayerFlags playerFlags;
//#define oPlayerName 0x650 // protected string _displayName;
//#define oPlayerHealth 0x20C // private float _health;
//#define olastSentTickTime 0x5CC // не требо private float lastSentTickTime;
//#define oClientTeam 0x540 // public PlayerTeam clientTeam;
//#define oLifestate 0x204 // public BaseCombatEntity.LifeState lifestate;
//#define oSteamID 0x640 // public ulong userID;
//#define oPlayerInventory 0x608 // public PlayerInventory inventory;
//#define oActiveUID 0x570// private uint clActiveItem;
//#define oFrozen 0x4B0// public bool Frozen;
//#define oWaterBonus 0x6B8// public float clothingWaterSpeedBonus;
//#define oNoBlockAiming 0x6B0 // public bool clothingBlocksAiming;
//#define oSpeedReduction 0x6B4// public float clothingMoveSpeedReduction;
//#define oClothingAccuracyBonus 0x6BC //	public float clothingAccuracyBonus;
////1
//
////EntityRef
//#define oHeld 0x98 //private EntityRef heldEntity
//#define oStancePenalty 0x304 //private float stancePenalty;
//#define	oAimconePenalty	0x308 //private float aimconePenalty;
//#define	oHipAimCone 0x2D4 //public float hipAimCone;
//#define	oAimCone 0x2D0 //public float aimCone;			(class BaseProjectile)
//#define	oAimconePenaltyPerShot 0x2D8 //public float aimconePenaltyPerShot;
//#define oSuccessFraction 0x340 // public float successFraction;
//#define oAttackRadius 0x27C //public float attackRadius;
//#define oEffectiveRange 0x1E4 //public float effectiveRange;
//#define oIsAutomatic 0x280 //public bool isAutomatic;
//#define oMaxDistance 0x278 //public float maxDistance;			(class BaseMelee)
//#define oRepeatDelay 0x1DC //public float repeatDelay;
//
////BaseProjectile
//#define oAuto 0x270 //public bool automatic;
//#define oFastReload 0x2A8 // bool fractionalReload
//#define oDistance 0x268 // public float distanceScale
//#define oprojectileVelocityScale 0x26C //public float projectileVelocityScale
//
////BaseMovement
//#define oPlayerMovement 0x4D0 // public BaseMovement movement;
//#define oGravityMultiplier 0x74 // public float gravityMultiplier;
//
//#define oGroundAngleNew 0xB8 // private float groundAngleNew;
//
////RecoilProperties
//#define oRecoilProperties 0x2C0 //public RecoilProperties recoil;
//#define oRecoilMinYaw 0x18 //public float recoilYawMin;
//#define oRecoilMaxYaw 0x1C //public float recoilYawMax;
//#define oRecoilMinPitch 0x20 //public float recoilPitchMin;
//#define oRecoilMaxPitch 0x24 //public float recoilPitchMax;
//
//
////PlayerModel
//#define oPlayerModel 0x4A8 // public PlayerModel playerModel;
//#define oVisible 0x248 // internal bool visible;
//#define oHeldEntity 0x98// private EntityRef heldEntity;
//
////PlayerInput
//#define oPlayerInput 0x4C8// public PlayerInput input;
//#define oBodyAngles 0x3C // private Vector3 bodyAngles;
//#define oRecoilAngles 0x64 // public Vector3 recoilAngles;
//#define oHasKeyFocus 0x94 // private bool hasKeyFocus;
//
//
//#define oArrowBack 0x344 // private float arrowBack;
//#define oTriggerReady 0x340 // protected bool attackReady;
//#define oItemDefinition 0x20 // public ItemDefinition info;
//#define oItemid 0x18 // public int itemid;