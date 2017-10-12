#pragma once
#include "F4SE/GameMenus.h"
#include "F4SE/GameData.h"
#include "F4SE/GameReferences.h"
#include "F4SE/GameExtraData.h"
#include "F4SE/GameRTTI.h"
#include "F4SE/GameInput.h"


template<typename T>
inline void Register(GFxValue * dst, const char * name, T value)
{

}

template<>
inline void Register(GFxValue * dst, const char * name, SInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, UInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetUInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, bool value)
{
	GFxValue	fxValue;
	fxValue.SetBool(value);
	dst->SetMember(name, &fxValue);
}

inline void RegisterString(GFxValue * dst, GFxMovieRoot * view, const char * name, const char * str)
{
	GFxValue	fxValue;
	view->CreateString(&fxValue, str);
	dst->SetMember(name, &fxValue);
}


class ButtonHintBar : public BSGFxShaderFXTarget
{
public:
	UInt64		unkB0[(0xD8 - 0xB0) >> 3];
};
STATIC_ASSERT(sizeof(ButtonHintBar) == 0xD8);

struct ViewCasterUpdateEvent
{
	UInt32			crosshairHandler;	// 00	RefHandler
	UInt32			unk04;				// 04
	UInt32			unk08;				// 08
	UInt32			unk0C;				// 0C
	UInt64			unk10;				// 10
	UInt64			unk18;				// 18
	UInt32			unk20;				// 20
	UInt32			unk24;				// 24
	bool			unk28;				// 28
	UInt32			unk2C;				// 2C
	bool			isNPC;				// 30 ?
	float			unk34;				// 34
	bool			canInteractive;		// 38
};
STATIC_ASSERT(sizeof(ViewCasterUpdateEvent) == 0x40);


class ViewCasterUpdateEventSource// : public BSTEventSink<BSTValueRequestEvent<ViewCasterUpdateEvent>> //
{
public:
	//void			** vtbl; //02D27530
	DEFINE_MEMBER_FUNCTION(GetCrosshairRefHandle, UInt32 *, 0x9DDC60, UInt32& refHandle); // called by readyweaponhandler::processbuttonevent.
};
extern RelocPtr<ViewCasterUpdateEventSource *>	g_viewCasterUpdateEventSource;

//0x168
class TESKey : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_KEYM };

	TESFullName					fullName;			// 68
	BGSModelMaterialSwap		materialSwap;		// 78
	TESIcon						icon;				// B8
	TESValueForm				value;				// C8
	TESWeightForm				weight;				// D8
	BGSDestructibleObjectForm	destructible;		// E8
	BGSMessageIcon				messageIcon;		// F8
	BGSPickupPutdownSounds		pickupSounds;		// 110
	BGSKeywordForm				keyword;			// 128
	BGSFeaturedItemMessage		featuredMessage;	// 148
	UInt64						unk158[2];			// 158
};
STATIC_ASSERT(sizeof(TESKey) == 0x168);



class TESObjectBOOK : public TESBoundObject
{

public:
	enum { kTypeID = kFormType_BOOK };

	enum	// type - these are technically flags
	{
		kType_None = 0,
		kType_Skill = 1 << 0,	// 01
		kType_CantBeTaken = 1 << 1,	// 02
		kType_Spell = 1 << 2,	// 04 takes priority over skill
		kType_Read = 1 << 3,	// 08 set once the book is equipped by the player, along with the CHANGE_BOOK_READ (0x40) change flag
		kType_AddPerk = 1 << 4
	};

	TESFullName					fullName;
	BGSModelMaterialSwap		materialSwap;
	TESIcon						icon;
	TESValueForm				value;
	TESWeightForm				weight;
	TESDescription				description; // E8
	BGSDestructibleObjectForm	destructible;
	BGSMessageIcon				messageIcon;
	BGSPickupPutdownSounds		pickupSounds;
	BGSKeywordForm				keyword;
	BGSFeaturedItemMessage		featuredMessage;

	UInt8						flags;					// 0x170
	BGSPerk						* unk178;
	UInt64						unk180;
	void						* unk188;				// TESObjectSTAT
	TESDescription				description2;
};
STATIC_ASSERT(sizeof(TESObjectBOOK) == 0x1A8);
STATIC_ASSERT(offsetof(TESObjectBOOK, description) == 0xE8);

//0x108
class BGSNote : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_NOTE };
	enum
	{
		kNote_SECN = 1,
		kNote_GAME,
		kNote_TERM
	};

	TESWeightForm				weight;				// 68
	TESValueForm				value;				// 78
	TESModel					model;				// 88
	TESFullName					fullName;			// B8
	TESIcon						icon;				// C8
	BGSPickupPutdownSounds		pickupSounds;		// D8
	BSFixedString				swfName;			// F0 //program name,maybe BSFixedString;
	UInt32						linkedFormID;		// F8 terminal or scene.
	UInt32						unkFC;				// FC padding
	UInt8						holotapeType;		// 100 scene: 1  swf: 2  terminal: 3
};
STATIC_ASSERT(sizeof(BGSNote) == 0x108);


class PipboyManager : public BSTEventSink<BSAnimationGraphEvent>,
					  public BSInputEventUser
{
public:
	UInt64						singleton[2]; // 18

	DEFINE_MEMBER_FUNCTION(PlayHolotape, void, 0xC1D140, BGSNote*, bool stop); // false. play with animation, without animation in power armor mode.
};
extern RelocPtr<PipboyManager *>  g_pipboyManager;//59F81A0


class PlayerInputHandler : public BSInputEventUser
{

};

//0x28
class ReadyWeaponHandler : public PlayerInputHandler
{
public:
	//void						** vtbl; //02D5FA48
	void						* unk10;
	bool						unk18;
	UInt32						unk1C;
	bool						unk20;
};

//0x20
class ExtraCount : public BSExtraData
{
public:

	enum{ kExtraTypeID = kExtraData_Count };

	ExtraCount();
	virtual ~ExtraCount();

	SInt16						count;
};

class ExtraReferenceHandle : public BSExtraData
{
public:
	enum { kExtraTypeID = kExtraData_ReferenceHandle };

	ExtraReferenceHandle();
	virtual ~ExtraReferenceHandle();

	UInt32						handle;		// 08
};

//0x38
class BGSBaseAlias;
class TESPackage;
class ExtraAliasInstanceArray : public BSExtraData
{
public:
	enum { kExtraTypeID = kExtraData_AliasInstanceArray };

	ExtraAliasInstanceArray();
	virtual ~ExtraAliasInstanceArray();

	struct AliasInfo //need confirm.
	{
		TESQuest			*	quest;
		BGSBaseAlias		*	alias;
		tArray<TESPackage*>	*	packages;
	};

	tArray<AliasInfo*>			aliases;	// 18
	UInt32						unk30;
	UInt32						unk34;
};
STATIC_ASSERT(sizeof(ExtraAliasInstanceArray) == 0x38);

struct ObjectInstanceData
{
	TESForm						* form;
	TBO_InstanceData			* data;
};

struct DamageInfo
{
	UInt32						type;
	float						damage;
};

struct ResistInfo
{
	UInt32						type;
	float						resist;
};

struct InventoryItemStack
{
	UInt64						unk00;
	UInt64						unk08;
	UInt64						unk10;
	ExtraDataList				* extraList;
};

struct InventoryItem
{
	TESForm						* form;
	InventoryItemStack			* stack;
};

template<typename T>
struct SimpleCollector
{
	UInt64						unk00;
	T							* info;
	UInt32						unk10;
	UInt64						count;
};

class HUDRollover : public HUDComponentBase
{
public:
	BSGFxShaderFXTarget			RolloverName_tf;				// 0x0F8
	ButtonHintBar				ButtonHintBar_mc;				// 0x1A8
	BSGFxShaderFXTarget			LegendaryIcon_mc;				// 0x280
	BSGFxShaderFXTarget			TaggedForSearchIcon_mc;			// 0x330
	UInt64						unk3E0[(0x5A8 - 0x3E0) >> 3];	// 0x3E0

	DEFINE_MEMBER_FUNCTION(UpdateElements, void, 0xAB1840, BSFixedString &); //called by updatecomponents;
	DEFINE_MEMBER_FUNCTION(ClearElements, void, 0xAB1AA0); //called by updatecomponents;
};
STATIC_ASSERT(sizeof(HUDRollover) == 0x5A8);


// 0x7F8
// ctor A89F00

class HUDQuickContainer : public HUDComponentBase
{
public:
	struct ItemData
	{
		BSFixedString			name;		//maybe...
		UInt32					count;
		UInt64					flags;
	};

	//void						** vtbl;							// 02D2E518
	BSGFxShaderFXTarget			ListHeaderAndBracket_mc;			// 0x0F8
	BSGFxShaderFXTarget			BracketPairHolder_mc;				// 0x1A8
	BSGFxShaderFXTarget			ListItems_mc;						// 0x258
	ButtonHintBar				ButtonHintBar_mc;					// 0x308
	UInt64						ItemDataA[(0x418 - 0x3E0) >> 3];	// 0x3E0
	UInt64						AButton[(0x4B0 - 0x418) >> 3];		// 0x418
	UInt64						XButton[(0x548 - 0x4B0) >> 3];		// 0x4B0
	UInt64						YButton[(0x5E0 - 0x548) >> 3];		// 0x548
	UInt64						unk5E0[(0x710 - 0x5E0) >> 3];		// 0x5E0

	//QuickContainerStateEvent int __fastcall sub_AA0D40(__int64 a1) source
	UInt32						flags;								// 0x710
	ItemData					itemData[5];						// 0x718  union?
	UInt32						dataLen;							// 0x790
	UInt64						unk798[(0x7A8 - 0x798) >> 3];		// 0x798
	BSFixedString				refName;							// 0x7A8
	UInt64						unk7B0;								// 0x7B0
	SInt32						selectedIndex;						// 0x7B8
	UInt64						unk7C0;								// 0x7C0
	bool						isDirty;							// 0x7C8
	UInt32						unk7CC;								// 0x7CC
	bool						unk7D0;								// 0x7D0
	UInt64						unk7D8[(0x7F8 - 0x7D8) >> 3];		// 0x7D8
	/*
	 26 +0D0 ptr: 0x0000020D134973C0: ?$BSTValueEventSink@VControlsRemappedEvent@@@@ *ptr: 0x00007ff6b59be500 | 0.000000, -0.000001:                        <no rtti>
	 27 +0D8 ptr: 0x0000020D134973C8:                        <no rtti> *ptr: 0x0000000000000000 | 0.000000, 0.000000:                           (null)
	 28 +0E0 ptr: 0x0000020D134973D0:                        <no rtti> *ptr: 0x0000000000000000 | 0.000000, 0.000000:                           (null)
	*/
};
STATIC_ASSERT(sizeof(HUDQuickContainer) == 0x7F8);
STATIC_ASSERT(offsetof(HUDQuickContainer, refName) == 0x7A8);
extern RelocAddr<void *>		g_HUDQuickContainerVTBL;

// 02D2EA78

class HUDQuickContainerDataModel : public BSInputEventUser
{
public:
	//A92680
	union DataOrPointer
	{
		SInt16		index;
		SInt16		* indexs;
	};
	struct Entry
	{
		UInt32			index;
		UInt32			unk04;
		UInt32			flags;
		UInt32			unk0C;
		DataOrPointer	stackIndex;
		UInt64			unk18;
	};
	UInt64						eventSinks[(0x40 - 0x10) >> 3];		// 0x010
	UInt64						unk040[(0xD0 - 0x40) >> 3];			// 0x040
	UInt32						refHandle;							// 0x0D0
	UInt64						unk0D8[(0xF0 - 0xD8) >> 3];			// 0x0D8
	UInt32						unk0F0;								// 0x0F0
	UInt32						type;								// 0x0F4
	UInt64						unk0F8[(0x210 - 0x0F8) >> 3];		// 0x0F8
	Entry						* entry;							// 0x210
	UInt64						unk218;								// 0x218
	UInt32						dataLen;							// 0x220
	UInt32						unk224;								// 0x224
	UInt8						unk228;								// 0x228
	UInt64						unk230[(0x270 - 0x230) >> 3];		// 0x230
	UInt32						unk270;								// 0x270
	SInt32						selectedIndex;						// 0x274
	UInt64						unk278;								// 0x278
	UInt8						unk280;								// 0x280
	bool						isPickUpKeyDown;					// 0x281
	//...

	DEFINE_MEMBER_FUNCTION(PickUpItem, void, 0xA921C0);
};
STATIC_ASSERT(sizeof(HUDQuickContainerDataModel) == 0x288);
STATIC_ASSERT(offsetof(HUDQuickContainerDataModel, isPickUpKeyDown) == 0x281);
STATIC_ASSERT(sizeof(HUDQuickContainerDataModel::Entry) == 0x20);
STATIC_ASSERT(offsetof(HUDQuickContainerDataModel, unk224) == 0x224);
STATIC_ASSERT(offsetof(HUDQuickContainerDataModel, refHandle) == 0x0D0);
STATIC_ASSERT(offsetof(HUDQuickContainerDataModel, type) == 0x0F4);
//AB5F70
/*
VTBL: 02D2F838	OFFSET : 0010	CLASS : . ? AVHUDRolloverModel@@
	>> DEPTH: 4	OFFSET : 0000	CLASS : . ? AVHUDRolloverModel@@
	>> DEPTH: 0	OFFSET : 0020	CLASS : . ? AVUIDebugLogger_NoOp@nsUIDebug@@
	>> DEPTH: 0	OFFSET : 0000	CLASS : . ? AV ? $BSTEventSink@UTESCommandModeEnterEvent@@@@
	>> DEPTH: 0	OFFSET : 0008	CLASS : . ? AV ? $BSTEventSink@UTESCommandModeExitEvent@@@@
	>> DEPTH: 0	OFFSET : 0010	CLASS : . ? AVBSInputEventUser@@
	*/

class HUDRolloverModel
{
public:
	UInt64						unk00[0x20 >> 3];					// 0x000
	HUDQuickContainerDataModel	quickContainerDataModel;			// 0x020
	//....
};

//A51780
class HUDDataModel
{
public:
	UInt64						singleton[0x18 >> 3];				// 0x000
	HUDRolloverModel			rolloverModel;						// 0x018 + 0x020
	//...
	inline HUDQuickContainerDataModel * GetQCData()
	{
		return &rolloverModel.quickContainerDataModel;
	}
};


struct HUDModeEvent
{
	tArray<BSFixedString>	* modes;
};

class HUDModeEventSource
{
public:
	UInt64									singleton[2];
	BSTEventDispatcher<HUDModeEvent>		source;
};

//extern RelocAddr<HUDQuickContainerDataModel *>	g_qucikContainerDataModel; //5A81318
extern RelocPtr<HUDDataModel *>					g_HUDDataModel;
extern RelocPtr<HUDModeEventSource *>			g_HUDModeEventSource; //5A7F200

typedef void(*_PlayHolotape)(BGSNote *);
extern RelocAddr<_PlayHolotape>			PlayHolotape; // called by pipboy menu.

typedef void(*_DisplayBookMenu)(BSString & description, ExtraDataList *, TESObjectREFR * pRef, TESObjectBOOK*, NiPoint3 & pos, NiMatrix43 & rot, float scale, bool showTakeButton);  //true;
extern RelocAddr<_DisplayBookMenu>		DisplayBookMenu;

using _CalcInstanceData = ObjectInstanceData *(*)(ObjectInstanceData & out, TESForm *, TBO_InstanceData *);
extern RelocAddr<_CalcInstanceData>		CalcInstanceData; ///V1.10.26