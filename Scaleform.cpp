#include "Defination.h"
#include "Scaleform.h"
#include "HookUtil.h"
#include "ModSettings.h"

#include "F4SE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "F4SE_common/BranchTrampoline.h"

#include "F4SE/InputMap.h"
#include "F4SE/NiNodes.h"
#include "F4SE/Translation.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

#pragma execution_character_set("utf-8")

#define ITEM_WEIGHT				"$wt"
#define ITEM_VALUE				"$val"
#define ITEM_RATIO				"$VALUE/WEIGHT"
#define WEAPON_DPS				"$DPS"	
#define BURDEN_STATE			"$CARRYWEIGHT"	

#define SetGFxMember(objName, keyName, format,  ...)								\
	sprintf_s(sResult.get(), MAX_PATH, (format), __VA_ARGS__);						\
	GFxValue _##objName##_;															\
	pRoot->CreateObject(&_##objName##_);											\
	RegisterString(&_##objName##_, pRoot, "key", (keyName));						\
	RegisterString(&_##objName##_, pRoot, "value", sResult.get());					\
	pInfo->PushBack(&_##objName##_);	

#define RegisterBriefItemInfo()														\
	GFxValue briefItemInfo;															\
	pRoot->CreateObject(&briefItemInfo);											\
	RegisterString(&briefItemInfo, pRoot, "itemWeight", strstream.str().c_str());	\
	Register<UInt32>(&briefItemInfo, "itemValue", itemValue);						\
	pExtraInfo->SetMember("briefItemInfo", &briefItemInfo);	

#define RegisterItemRatioInfo()														\
	if (itemWeight != 0.0f){														\
	std::stringstream().swap(strstream);											\
	strstream << std::fixed << std::setprecision(GetPrecision(static_cast<float>(itemValue) / itemWeight)) << static_cast<float>(itemValue) / itemWeight;\
	SetGFxMember(ratio, ITEM_RATIO, "%s", strstream.str().c_str());}else{			\
	SetGFxMember(ratio, ITEM_RATIO, "%s", "-");}

SimpleLock						g_quickContainerLock;


static void OnCrosshairRefChange(GFxMovieRoot * pRoot, GFxValue * pInfo, GFxValue * pExtraInfo, /*TESObjectREFR* pRef, */TESForm * pBaseForm, ExtraDataList * pExtraDataList, UInt32 itemCount)
{
	if (!pRoot || !pInfo || !pExtraInfo || !pBaseForm || !pExtraDataList)	return;
	//ExtraDataList * pExtraDataList = pRef->extraDataList;
	auto pExtraCount = DYNAMIC_CAST(pExtraDataList->GetExtraData(kExtraData_Count), BSExtraData, ExtraCount); //sometimes the result form staic_cast isn't correct and causes game freeze, so use dynamic_cast to ensure safety.
	itemCount = (pExtraCount != nullptr && pExtraCount->count > itemCount) ? pExtraCount->count : itemCount;

	//auto pBaseForm = pRef->baseForm;
	std::unique_ptr<char[]>	sResult(new char[MAX_PATH]);

	auto GetPrecision = [](float param)->UInt8
	{
		return (static_cast<UInt32>(abs(param) * 100) % 10 == 0) ? ((static_cast<UInt32>(abs(param) * 10) % 10 == 0) ? 0 : 1) : 2;
	};

	std::stringstream strstream;

	Register<UInt32>(pExtraInfo, "type", pBaseForm->formType);
	Register<UInt32>(pExtraInfo, "formID", pBaseForm->formID);

	switch (pBaseForm->formType)
	{
	case FormType::kFormType_MISC:
	{
		auto pMiscItem = static_cast<TESObjectMISC*>(pBaseForm);
		if (pMiscItem->components)
		{
			RelocAddr<bool(*)(void *, BGSComponent*)> IsTaggedForSearch = 0x126E760;			/// v1.10.26
			RelocPtr<void*>	pTaggedComponents = 0x5A7E020;										/// v1.10.26
			for (size_t i = 0; i < pMiscItem->components->count; ++i)
			{
				BGSComponent * pComponent = pMiscItem->components->operator[](i).component;
				GFxValue component;
				pRoot->CreateObject(&component);
				sprintf_s(sResult.get(), MAX_PATH, "x%d", pMiscItem->components->operator[](i).count);
				RegisterString(&component, pRoot, "key", pComponent->fullName.GetFullName());
				RegisterString(&component, pRoot, "value", sResult.get());
				Register<bool>(&component, "isComponent", true);
				Register<bool>(&component, "isTagged", IsTaggedForSearch((*pTaggedComponents), pComponent));
				pInfo->PushBack(&component);
			}
		}
		UInt32 itemValue = pMiscItem->value.value * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		if (pMiscItem->formID == 0xF)
		{
			SetGFxMember(caps, "$CAPS", "%d", (*g_player)->GetItemCount(pMiscItem));
		}
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		//BURDEN_STATE
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));

		RegisterItemRatioInfo();
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_ALCH:
	{
		auto pAlchemyItem = static_cast<AlchemyItem*>(pBaseForm);

		RelocPtr<BGSKeyword*>	pSyringerAmmoKW = 0x59016D0;///V1.10.26 ObjectTypeSyringerAmmo [KYWD:0018506F]
		if (!pAlchemyItem->keywordForm.keywordBase.HasKeyword(*pSyringerAmmoKW))
		{
			float HP = 0.0f;
			for (size_t i = 0; i < pAlchemyItem->effectItems.count; ++i)
			{
				auto pEffectItem = pAlchemyItem->effectItems[i];
				auto pEffectSetting = pEffectItem->mgef;
				if (pEffectSetting != nullptr && !(pEffectSetting->effectType & EffectSetting::kEffectType_HideInUI)	\
					&& EvaluationConditions(&pEffectItem->conditions, *g_player, *g_player)								\
					&& EvaluationConditions(&pEffectSetting->conditions, *g_player, *g_player))
				{
					RelocAddr<void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration)> CalcMagicEffect = 0xBC2D70;///V1.10 CalaPerkEffect in this funtion.BC2D70
					float magnitude = 0.0f, duration = 0.0f;
					CalcMagicEffect(pAlchemyItem, pEffectItem, magnitude, duration);
					RelocAddr<float(*)(MagicItem*, MagicItem::EffectItem*)> GetRestoredHPPercent = 0x12BF570; ///V1.10.26

					MagicItem::EffectItem::Description modifiedValueName{ nullptr, 0, 0, 0, 0 };
					MagicItem::EffectItem::Description effectDescription{ nullptr, 0, 0, 0, 0 };

					RelocAddr<void(*)(MagicItem::EffectItem*, MagicItem::EffectItem::Description&)> GetModifiedValueName = 0xC0AE60; ///V1.10.26 C0AE60
					GetModifiedValueName(pEffectItem, modifiedValueName);

					if (modifiedValueName.pDescription && modifiedValueName.pDescription[0] && magnitude != 0.0f)
					{
						GFxValue actorValue;
						pRoot->CreateObject(&actorValue);
						RegisterString(&actorValue, pRoot, "key", modifiedValueName.pDescription);
						Register<UInt32>(&actorValue, "duration", static_cast<UInt32>(duration));
						std::stringstream().swap(strstream);
						strstream << std::fixed << std::setprecision(GetPrecision(magnitude)) << magnitude;

						ActorValueInfo * pActorValue = pEffectSetting->actorValInfoD8;
						RelocAddr<ActorValueInfo**(*)()> GetActorValueCollector = 0x006B1F0;
						auto pAVs = GetActorValueCollector();
						if (pEffectSetting->archetype == EffectSetting::kArchetype_Stimpak \
							|| pActorValue == *reinterpret_cast<ActorValueInfo**>(GetActorValueCollector() + 0x1A))
						{
							strstream << "%";
							Register<bool>(&actorValue, "showAsPercent", true);
						}
						if (duration >= 1.0f && !(pEffectSetting->effectType & EffectSetting::kEffectType_Recover) \
							&& pEffectSetting->archetype != EffectSetting::kArchetype_Stimpak/*pActorValue && pActorValue->formID == 0x2E1 || pActorValue->formID == 0x2D4 || pActorValue->formID == 0x2D5*/)
						{
							strstream << "/s";
							Register<bool>(&actorValue, "scaleWithDuration", true);
						}
						RegisterString(&actorValue, pRoot, "magnitude", strstream.str().c_str());
						//RegisterString(&actorValue, pRoot, "value", strstream.str().c_str()); //debug.
						pInfo->PushBack(&actorValue);
					}
					else
					{
						pEffectItem->CalcDescription(effectDescription, "", "", magnitude, duration); ///V1.10.26
						if (effectDescription.pDescription && effectDescription.pDescription[0])
						{
							if (effectDescription.pDescription[0] == 0x25 || effectDescription.pDescription[0] == 0x24)
							{
								GFxValue actorValue;
								pRoot->CreateObject(&actorValue);
								RegisterString(&actorValue, pRoot, "key", effectDescription.pDescription + 1);
								Register<UInt32>(&actorValue, "duration", static_cast<UInt32>(duration));

								std::stringstream().swap(strstream);
								strstream << std::fixed << std::setprecision(GetPrecision(magnitude)) << magnitude;
								if (effectDescription.pDescription[0] == 0x25) //'%'
								{
									strstream << "%";
									Register<bool>(&actorValue, "showAsPercent", true);
								}
								if (duration >= 1.0f && !(pEffectSetting->effectType & EffectSetting::kEffectType_Recover) \
									&& pEffectSetting->archetype != EffectSetting::kArchetype_Stimpak)
								{
									strstream << "/s";
									Register<bool>(&actorValue, "scaleWithDuration", true);
								}
								RegisterString(&actorValue, pRoot, "magnitude", strstream.str().c_str());
								//RegisterString(&actorValue, pRoot, "value", strstream.str().c_str()); //debug.
								pInfo->PushBack(&actorValue);
							}
							else if (!pExtraInfo->HasMember("description"))
							{
								RegisterString(pExtraInfo, pRoot, "description", effectDescription.pDescription);
							}
						}
					}
					if (modifiedValueName.pDescription != nullptr)
					{
						Heap_Free(const_cast<char*>(modifiedValueName.pDescription));
					}
					if (effectDescription.pDescription != nullptr)
					{
						Heap_Free(const_cast<char*>(effectDescription.pDescription));
					}
					//HP += GetRestoredHPPercent(pAlchemyItem, pEffectItem);
					/*
					bool showDuration = false, showAsPercent = false, showAsDescription = false;

					RelocAddr<void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration)> CalcMagicEffect = 0xBC2D70;///V1.10 CalaPerkEffect in this funtion.BC2D70
					float magnitude = 0.0f, duration = 0.0f;
					CalcMagicEffect(pAlchemyItem, pEffectItem, magnitude, duration);
					RelocAddr<float(*)(MagicItem*, MagicItem::EffectItem*)> GetRestoredHPPercent = 0x12BF570; ///V1.10.26
					HP += GetRestoredHPPercent(pAlchemyItem, pEffectItem);

					MagicItem::EffectItem::Description modifiedValueName{ nullptr, 0, 0, 0, 0 };
					MagicItem::EffectItem::Description description{ nullptr, 0, 0, 0, 0 };

					const char * pModifiedValueName = nullptr, * pDescription = nullptr;
					RelocAddr<void(*)(MagicItem::EffectItem*, MagicItem::EffectItem::Description&)> GetModifiedValueName = 0xC0AE60; ///V1.10.26 C0AE60
					GetModifiedValueName(pEffectItem, modifiedValueName);
					pModifiedValueName = modifiedValueName.pDescription;
					if (duration > 0.0f && !(pEffectSetting->effectType & EffectSetting::kEffectType_NoRecover))
					showDuration = true;

					if (pEffectSetting->archetype == EffectSetting::kArchetype_ValueAndParts)
					{
					showAsPercent = true;
					showDuration = false;
					}
					else
					{
					ActorValueInfo * pModifiedValue = pEffectSetting->actorValInfoD8;
					RelocAddr<ActorValueInfo**(*)()> GetActorValueContainer = 0x006B1F0;
					auto pAVs = GetActorValueContainer();
					ActorValueInfo*	pXP = *reinterpret_cast<ActorValueInfo**>(pAVs + 0x1A);
					if (pXP == pModifiedValue)
					showAsPercent = true;
					else if (!pModifiedValue || !modifiedValueName.pDescription || !modifiedValueName.pDescription[0])
					{
					pEffectItem->CalcDescription(description, "", "", magnitude, duration); ///V1.10.26
					if (description.pDescription && description.pDescription[0])
					{
					if (description.pDescription[0] != 0x25 && description.pDescription[0] != 0x24)
					{
					pDescription = description.pDescription;
					showAsDescription = true;
					}
					else
					{
					showAsPercent = (description.pDescription[0] == 0x25) ? true : showAsPercent;
					pModifiedValueName = description.pDescription + 1;
					_MESSAGE("%s| %d", pModifiedValueName, showDuration);
					}
					}
					}
					}
					if (pModifiedValueName && pModifiedValueName[0] && magnitude != 0.0f)
					{
					GFxValue actorValue;
					pRoot->CreateObject(&actorValue);
					RegisterString(&actorValue, pRoot, "key", pModifiedValueName);
					std::stringstream().swap(strstream);
					strstream << std::fixed << std::setprecision(GetPrecision(magnitude)) << magnitude;
					if (showAsPercent)
					{
					_MESSAGE("show percent");
					strstream << "%";
					}
					RegisterString(&actorValue, pRoot, "value", strstream.str().c_str());
					if (showDuration)
					{
					Register<UInt32>(&actorValue, "duration", static_cast<UInt32>(duration));
					}
					pInfo->PushBack(&actorValue);
					}
					else if (showAsDescription && pDescription && pDescription[0] && !pExtraInfo->HasMember("showAsDescription"))
					{
					RegisterString(pExtraInfo, pRoot, "showAsDescription", pDescription);
					}
					if (modifiedValueName.pDescription != nullptr)
					{
					Heap_Free(const_cast<char*>(modifiedValueName.pDescription));
					}
					if (description.pDescription != nullptr)
					{
					Heap_Free(const_cast<char*>(description.pDescription));
					}*/
				}
			}
		}
		else
		{
			for (size_t i = 0; i < pAlchemyItem->effectItems.count; ++i)
			{
				auto pEffectItem = pAlchemyItem->effectItems[i];
				auto pEffectSetting = pEffectItem->mgef;
				MagicItem::EffectItem::Description description{ nullptr, 0, 0, 0, 0 };
				RelocAddr<void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration)> CalcMagicEffect = 0xBC2D70;///V1.10.26 CalaPerkEffect in this funtion.
				float magnitude = 0.0f, duration = 0.0f;
				CalcMagicEffect(pAlchemyItem, pEffectItem, magnitude, duration);
				pEffectItem->CalcDescription(description, "", "", magnitude, duration); ///V1.10.26
				if (description.pDescription && description.pDescription[0] && !pExtraInfo->HasMember("description"))
				{
					RegisterString(pExtraInfo, pRoot, "description", description.pDescription);
				}
				if (description.pDescription != nullptr)
				{
					Heap_Free(const_cast<char*>(description.pDescription));
				}
			}
		}
		if (!pExtraInfo->HasMember("description"))
		{
			BSString str;
			pAlchemyItem->description.Get(str, nullptr);
			if (str.m_data && str.m_data[0])
			{
				RegisterString(pExtraInfo, pRoot, "description", str.Get());
			}
		}

		UInt32 itemValue = pAlchemyItem->value * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_AMMO:
	{
		auto pAmmo = static_cast<TESAmmo*>(pBaseForm);
		RelocAddr<BGSKeyword*(*)()> GetChargeKW = 0x1E29B0; ///V1.10.26
		BGSKeyword * pKW = GetChargeKW(); //Charge keyword.
		if (pAmmo->keywordForm.keywordBase.HasKeyword(pKW))
		{
			float currentCharge = pExtraDataList->GetCharge();//
			//_MESSAGE(">> ammo charge: %08X", currentCharge);
			if (currentCharge <= 0.0f)
				currentCharge = 1.0f;
			SetGFxMember(charge, "$charge", "%d/%d", static_cast<UInt32>(currentCharge * (*RelocAddr<float*>(0x3748848))), static_cast<UInt32>(*RelocAddr<float*>(0x3748848)));
		}

		UInt32 itemValue = pAmmo->value.value * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		if (itemWeight > 0.0f && itemWeight < 0.01f)
		{
			std::stringstream().swap(strstream);
			strstream << std::fixed << std::setprecision(3) << itemWeight;
		}
		else
		{
			std::stringstream().swap(strstream);
			strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		}
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_BOOK:
	{
		auto pBook = static_cast<TESObjectBOOK*>(pBaseForm);
		UInt32 itemValue = pBook->value.value * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if ((pBook->flags & TESObjectBOOK::kType_Read) == TESObjectBOOK::kType_Read)
		{
			Register<bool>(pExtraInfo, "isRead", true);
		}
		if ((pBook->flags & TESObjectBOOK::kType_AddPerk) == TESObjectBOOK::kType_AddPerk)
		{
			Register<bool>(pExtraInfo, "isSkillBook", true);
		}
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_NOTE:
	{
		auto pNote = static_cast<BGSNote*>(pBaseForm);
		UInt32 itemValue = pNote->value.value * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_WEAP:
	{
		auto pWeapon = static_cast<TESObjectWEAP*>(pBaseForm);

		ObjectInstanceData objInst{ nullptr, nullptr };
		//CalcInstanceData(objInst, pRef, nullptr);
		CalcInstanceData(objInst, pWeapon, pExtraDataList->GetExtraInstanceData());
		if (objInst.data && !Runtime_DynamicCast(objInst.data, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData))
		{
			objInst.data->DecRefHandle();
			objInst.data = nullptr;
		}
		auto pWeaponInst = (TESObjectWEAP::InstanceData*)((objInst.data != nullptr) ? objInst.data : pWeapon->GetInstanceData());

		SimpleCollector<DamageInfo>		damageInfo{ 0, nullptr, 0, 0 };
		InventoryItemStack				stack{ 0, 0, 0, pExtraDataList };
		RelocAddr<void(*)(TESForm *&, InventoryItemStack &, SimpleCollector<DamageInfo>&)> CollectDamageInfo = 0xC08CC0; ///V1.10.26
		CollectDamageInfo(pBaseForm, stack, damageInfo);

		for (size_t i = 0; i < damageInfo.count; ++i)
		{
			if (damageInfo.info[i].damage != 0.0f)
			{
				GFxValue info;
				pRoot->CreateObject(&info);
				Register<UInt32>(&info, "damageType", damageInfo.info[i].type);
				Register<bool>(&info, "isWeapon", true);
				sprintf_s(sResult.get(), MAX_PATH, "%d", static_cast<UInt32>(damageInfo.info[i].damage));
				RegisterString(&info, pRoot, "value", sResult.get());
				pInfo->PushBack(&info);
			}
		}
		if (damageInfo.info != nullptr)
		{
			RelocAddr<void(*)(SimpleCollector<DamageInfo>&)>	Release = 0x1B0F4F0; ///V1.10.26
			Release(damageInfo);
		}

		TESAmmo* pAmmo = (pWeaponInst != nullptr) ? pWeaponInst->ammo : pWeapon->weapData.ammo;
		if (pAmmo != nullptr)
		{
			bool isJunk = pAmmo->IsJunk();
			RelocAddr<const char *(*)(UInt64 &)> GetCRCString = 0x1B2E10; ///V1.10.26
			GFxValue ammo;
			pRoot->CreateObject(&ammo);
			RegisterString(&ammo, pRoot, "key", GetCRCString(pAmmo->crcString));
			Register<bool>(&ammo, "isAmmo", true);
			if (isJunk && (*g_player)->unkD80)
				RegisterString(&ammo, pRoot, "value", "-");
			else
			{
				UInt32 ammoCount = (*g_player)->GetItemCount(pAmmo);
				sprintf_s(sResult.get(), MAX_PATH, "%d", ammoCount);
				RegisterString(&ammo, pRoot, "value", sResult.get());
			}
			pInfo->PushBack(&ammo);
		}

		if (pWeapon->weapData.weaponType == TESObjectWEAP::InstanceData::kWeaponType_Gun)  //weapon type
		{
			float speed = pWeapon->GetShootSpeed(pWeaponInst);///V1.10.26
			SetGFxMember(speed, "$ROF", "%d", static_cast<UInt32>(speed));
			RelocAddr<float(*)(ObjectInstanceData&)> GetWeaponRange = 0x661C40;///V1.10.26
			float range = GetWeaponRange(objInst);
			SetGFxMember(range, "$rng", "%d", static_cast<UInt32>(range));
			RelocAddr<float(*)(ObjectInstanceData&, Actor*)> GetWeaponAccuracy = 0x661D20; ///V1.10.26
			try
			{
				float accuracy = GetWeaponAccuracy(objInst, (*g_player));
				SetGFxMember(accuracy, "$acc", "%d", static_cast<UInt32>(accuracy));
			}
			catch (...)
			{
				_MESSAGE("failed to get accuracy of weapon | %08X", pWeapon->formID);
			}
		}
		else if (pWeapon->weapData.weaponType <= TESObjectWEAP::InstanceData::kWeaponType_TwoHandedAxe)
		{
			UInt32 attackSpeed = pWeapon->GetAttackSpeed();
			RelocAddr<const char * (*)(UInt32)>	GetWeaponSpeedDesc = 0x34D330; ///V1.10.26
			const char * pDesc = GetWeaponSpeedDesc(attackSpeed);
			SetGFxMember(speed, "$speed", "%s", pDesc);
		}

		UInt32 itemValue = pBaseForm->GetValue(pExtraDataList) * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(objInst.data) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (objInst.data != nullptr)
		{
			objInst.data->DecRefHandle();
			objInst.data = nullptr;
		}
		BGSMod::Attachment::Mod * pMod = pExtraDataList->GetEffectiveMod();
		if (pMod != nullptr)
		{
			BSString str;
			TESDescription * pDescription = DYNAMIC_CAST(pMod, TESForm, TESDescription);
			pDescription->Get(str, nullptr);
			RegisterString(pExtraInfo, pRoot, "description", str.Get());
		}
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_ARMO:
	{
		auto pArmor = static_cast<TESObjectARMO*>(pBaseForm);
		ObjectInstanceData objInst{ nullptr, nullptr };
		CalcInstanceData(objInst, pArmor, pExtraDataList->GetExtraInstanceData());
		if (objInst.data && !Runtime_DynamicCast(objInst.data, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData))
		{
			objInst.data->DecRefHandle();
			objInst.data = nullptr;
		}
		auto pArmorInst = (TESObjectARMO::InstanceData*)((objInst.data != nullptr) ? objInst.data : pArmor->GetInstanceData());

		SimpleCollector<ResistInfo>		resistInfo{ 0, nullptr, 0, 0 };
		InventoryItemStack				stack{ 0, 0, 0, pExtraDataList };
		InventoryItem					item{ pBaseForm, &stack };

		for (size_t i = 0; pArmorInst->valueModifier && i < pArmorInst->valueModifier->count; ++i)
		{
			auto valueModifier = pArmorInst->valueModifier->operator[](i);
			const char * pDisplayName = valueModifier.avInfo->GetDisplayName();
			if (pDisplayName && pDisplayName[0])
			{
				std::stringstream().swap(strstream);
				strstream << std::fixed << std::setprecision(GetPrecision(valueModifier.unk08)) << valueModifier.unk08;
				SetGFxMember(actorValue, pDisplayName, "%s", strstream.str().c_str());
			}
		}

		RelocAddr<void(*)(InventoryItem&, InventoryItemStack &, SimpleCollector<ResistInfo>&, float)> CollectResistInfo = 0xC09120; ///V1.10.26
		CollectResistInfo(item, stack, resistInfo, 1.0f);
		for (size_t i = 0; i < resistInfo.count; ++i)
		{
			if (resistInfo.info[i].resist != 0.0f)
			{
				GFxValue info;
				pRoot->CreateObject(&info);
				Register<UInt32>(&info, "damageType", resistInfo.info[i].type);
				Register<bool>(&info, "isArmor", true);
				sprintf_s(sResult.get(), MAX_PATH, "%d", static_cast<UInt32>(resistInfo.info[i].resist));
				RegisterString(&info, pRoot, "value", sResult.get());
				pInfo->PushBack(&info);
			}
		}
		if (resistInfo.info != nullptr)
		{
			RelocAddr<void(*)(SimpleCollector<ResistInfo>&)>	Release = 0x1B0F4F0; ///V1.10.26
			Release(resistInfo);
		}

		RelocAddr<BGSKeyword*(*)()> GeHealthtKW = 0x1E2A10; ///V1.10.26
		BGSKeyword * pKW = GeHealthtKW(); ///Health keyword.
		if (pArmor->keywordForm.keywordBase.HasKeyword(pKW))
		{
			UInt32 fullHealth = pArmor->GetFullHealth(pArmorInst);///V1.10		
			UInt32 currentHealth = (pExtraDataList != nullptr) ? static_cast<UInt32>(pExtraDataList->GetCurrentHealth(pArmor)) : fullHealth;	 ///V1.10	
			SetGFxMember(health, "$health", "%d/%d", currentHealth, fullHealth);
		}

		TESEnchantableForm * pEnchantableForm = DYNAMIC_CAST(pArmor, TESForm, TESEnchantableForm); //still need confirm.
		if (pEnchantableForm != nullptr)
		{
			if (pArmorInst != nullptr)
			{
				tArray<EnchantmentItem*>* pEnchantmentItems = pArmorInst->GetEnchantmentItems();
				if (pEnchantmentItems != nullptr)
				{
					for (size_t i = 0; i < pEnchantmentItems->count; ++i)
					{
						auto pEnchantmentItem = pEnchantmentItems->operator[](i);
						for (size_t i = 0; i < pEnchantmentItem->effectItems.count; ++i)
						{
							auto pEffectItem = pEnchantmentItem->effectItems[i];
							if (pEffectItem->mgef && !(pEffectItem->mgef->effectType & EffectSetting::kEffectType_HideInUI) \
								&& EvaluationConditions(&pEffectItem->conditions, (*g_player), (*g_player)) \
								&& EvaluationConditions(&pEffectItem->mgef->conditions, (*g_player), (*g_player))\
								&& pEffectItem->GetMagnitude() != 0.0f && pEffectItem->mgef->actorValInfoD8)
							{
								const char * pDisplayName = pEffectItem->mgef->actorValInfoD8->GetDisplayName();
								if (pDisplayName && pDisplayName[0])
								{
									std::stringstream().swap(strstream);
									strstream << std::fixed << std::setprecision(GetPrecision(pEffectItem->magnitude)) << pEffectItem->magnitude;
									SetGFxMember(actorValue, pDisplayName, "%s", strstream.str().c_str());
								}
							}
						}
					}
				}
			}
			EnchantmentItem* pEnchantmentItem = pEnchantableForm->enchantment;
			if (pEnchantmentItem)
			{
				for (size_t i = 0; i < pEnchantmentItem->effectItems.count; ++i)
				{
					auto pEffectItem = pEnchantmentItem->effectItems[i];
					if (pEffectItem->mgef && !(pEffectItem->mgef->effectType & EffectSetting::kEffectType_HideInUI) \
						&& EvaluationConditions(&pEffectItem->conditions, (*g_player), (*g_player)) \
						&& EvaluationConditions(&pEffectItem->mgef->conditions, (*g_player), (*g_player))\
						&& pEffectItem->GetMagnitude() != 0.0f && pEffectItem->mgef->actorValInfoD8)
					{
						const char * pDisplayName = pEffectItem->mgef->actorValInfoD8->GetDisplayName();
						if (pDisplayName && pDisplayName[0])
						{
							std::stringstream().swap(strstream);
							strstream << std::fixed << std::setprecision(GetPrecision(pEffectItem->magnitude)) << pEffectItem->magnitude;
							SetGFxMember(actorValue, pDisplayName, "%s", strstream.str().c_str());
						}
					}
				}
			}
			pEnchantmentItem = pExtraDataList->GetExtraEnchantmentItem();
			if (pEnchantmentItem)
			{
				for (size_t i = 0; i < pEnchantmentItem->effectItems.count; ++i)
				{
					auto pEffectItem = pEnchantmentItem->effectItems[i];
					if (pEffectItem->mgef && !(pEffectItem->mgef->effectType & EffectSetting::kEffectType_HideInUI) \
						&& EvaluationConditions(&pEffectItem->conditions, (*g_player), (*g_player)) \
						&& EvaluationConditions(&pEffectItem->mgef->conditions, (*g_player), (*g_player))\
						&& pEffectItem->GetMagnitude() != 0.0f && pEffectItem->mgef->actorValInfoD8)
					{
						const char * pDisplayName = pEffectItem->mgef->actorValInfoD8->GetDisplayName();
						if (pDisplayName && pDisplayName[0])
						{
							std::stringstream().swap(strstream);
							strstream << std::fixed << std::setprecision(GetPrecision(pEffectItem->magnitude)) << pEffectItem->magnitude;
							SetGFxMember(actorValue, pDisplayName, "%s", strstream.str().c_str());
						}
					}
				}
			}
		}
		UInt32 itemValue = pBaseForm->GetValue(pExtraDataList) * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(objInst.data) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (objInst.data != nullptr)
		{
			objInst.data->DecRefHandle();
			objInst.data = nullptr;
		}
		BGSMod::Attachment::Mod * pMod = pExtraDataList->GetEffectiveMod();
		if (pMod != nullptr)
		{
			BSString str;
			TESDescription * pDescription = DYNAMIC_CAST(pMod, TESForm, TESDescription);
			pDescription->Get(str, nullptr);
			RegisterString(pExtraInfo, pRoot, "description", str.Get());
		}
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	case FormType::kFormType_KEYM:
	{
		auto pKey = static_cast<TESKey*>(pBaseForm);
		UInt32 itemValue = pKey->GetValue(pExtraDataList) * itemCount;
		SetGFxMember(golds, ITEM_VALUE, "%d", itemValue);
		float itemWeight = pBaseForm->GetWeight(nullptr) * itemCount;
		std::stringstream().swap(strstream);
		strstream << std::fixed << std::setprecision(GetPrecision(itemWeight)) << itemWeight;
		SetGFxMember(weight, ITEM_WEIGHT, "%s", strstream.str().c_str());
		RegisterBriefItemInfo();
		SetGFxMember(carryweight, BURDEN_STATE, "%d(+%s)/%d", static_cast<UInt32>((*g_player)->GetInventoryWeight()), strstream.str().c_str(), static_cast<UInt32>((*g_player)->GetCarryWeight()));
		RegisterItemRatioInfo();
		if (pExtraDataList->GetExtraData(kExtraData_AliasInstanceArray))
		{
			Register<bool>(pExtraInfo, "isQuestItem", true);
		}
		break;
	}
	}
	//other use.
	if (pExtraInfo->HasMember("description"))
	{
		GFxValue description;
		pExtraInfo->GetMember("description", &description);
		GFxValue descObj;
		pRoot->CreateObject(&descObj);
		descObj.SetMember("key", &description);
		Register<bool>(&descObj, "isDescription", true);
		pInfo->PushBack(&descObj);
		//_MESSAGE("desc=%s", description.GetString());
	}
}


class HUDRolloverEx : public HUDRollover
{
public:
	static GFxMovieView				* pGFxMovieView;
	static TESObjectREFR			* pCrosshairRef;

	void ClearElements_Hook()
	{
		//RelocAddr<BSFixedString*(*)()>	GetHUDName = 0x2049B10;
		HUDMenu * pHUD = nullptr;
		static BSFixedString menuName("HUDMenu");
		if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
		{
			GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;
			movieRoot->Invoke("root.HUDPlusPlusLoader.content.onCrosshairRefLeave", nullptr, nullptr, 0);
		}
		this->ClearElements();
	}

	void UpdateElements_Hook(BSFixedString & str)
	{
		this->UpdateElements(str);
		HUDMenu * pHUD = nullptr;
		static BSFixedString menuName("HUDMenu");
		if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
		{
			GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;
			UInt32	refHandle;
			TESObjectREFR * pRef = nullptr;
			if ((*g_viewCasterUpdateEventSource) != nullptr)
			{
				(*g_viewCasterUpdateEventSource)->GetCrosshairRefHandle(refHandle);
				LookupREFRByHandle(&refHandle, &pRef);
				if (pRef != nullptr)
				{
					GFxValue params[2];
					movieRoot->CreateArray(&params[0]);
					movieRoot->CreateObject(&params[1]);
					OnCrosshairRefChange(movieRoot, &params[0], &params[1], pRef->baseForm, pRef->extraDataList, 1);
					movieRoot->Invoke("root.HUDPlusPlusLoader.content.onCrosshairRefEnter", nullptr, params, 2);
					pRef->handleRefObject.DecRefHandle();
				}
			}
		}
	}

	static void InitHook()
	{
		g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0xAB1720), GetFnAddr(&UpdateElements_Hook));
		g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0xAB1727), GetFnAddr(&ClearElements_Hook));
	}

};
GFxMovieView								* HUDRolloverEx::pGFxMovieView = nullptr;
TESObjectREFR								* HUDRolloverEx::pCrosshairRef = nullptr;


class HUDQuickContainerEx : public HUDQuickContainer
{
public:
	//sub_A91040
	//sub_A8A4E0
	//QCTakeItem
	typedef void(__thiscall HUDQuickContainerEx::*FnUpdateElement)();
	static FnUpdateElement		fnUpdateElement;

	void UpdateElement_Hook()
	{
		if (this->unk7D0)
		{
			static BSFixedString menuName("HUDMenu");
			if (this->isDirty && this->selectedIndex >= 0 && this->dataLen && (*g_HUDDataModel) != nullptr)
			{
				auto pQC = (*g_HUDDataModel)->GetQCData();
				TESObjectREFR * pRef = nullptr;
				LookupREFRByHandle(&pQC->refHandle, &pRef);
				if (pRef != nullptr)
				{
					SimpleLocker locker(&g_quickContainerLock);
					bool tlsState[2];
					RelocAddr<void(*)(bool[], bool, bool)>  IncTLSRef = 0xEC0180;
					IncTLSRef(tlsState, true, false);
					if (/*(pQC->type == 0 || pQC->type == 6) && */pQC->dataLen && pQC->selectedIndex >= 0\
						&& !pQC->unk228 && pQC->selectedIndex < pQC->dataLen)
					{
						auto pEntry = &pQC->entry[pQC->selectedIndex];
						RelocAddr<TESForm*(*)(uintptr_t, HUDQuickContainerDataModel::Entry*)>	GetSelectedForm = 0x1A3740;
						auto pForm = GetSelectedForm((*RelocPtr<uintptr_t>(0x58F2D80)), pEntry);
						if (pForm != nullptr)
						{
							RelocAddr<BGSInventoryItem *(*)(uintptr_t, HUDQuickContainerDataModel::Entry*)>	GetSelectedInventoryItem = 0x1A3650;
							auto pItem = GetSelectedInventoryItem((*RelocPtr<uintptr_t>(0x58F2D80)), pEntry);
							SInt16 stackIndex = (pEntry->flags & 0x80000000) ? pEntry->stackIndex.index : (*pEntry->stackIndex.indexs);
							auto GetStack = [](BGSInventoryItem * pItem, SInt16 index)->BGSInventoryItem::Stack *
							{
								if (!pItem->stack)	return nullptr;
								auto pStack = pItem->stack;
								while (index-- && (pStack = pStack->next, pStack)) {}
								return pStack;
							};
							auto pStack = GetStack(pItem, stackIndex);
							UInt32 itemValue = 0, itemCount = 1;
							if (pStack != nullptr && pStack->extraData)
							{
								itemCount = pStack->count;
								itemValue = pForm->GetValue(pStack->extraData) * itemCount;
							}
							ObjectInstanceData objInst{ nullptr, nullptr };
							CalcInstanceData(objInst, pForm, (pStack && pStack->extraData) ? pStack->extraData->GetExtraInstanceData() : nullptr);
							float itemWeight = pForm->GetWeight(objInst.data) * itemCount;
							//weight = (*g_player)->CalcFormWeight(pForm, stack, weight, nullptr);
							if (objInst.data != nullptr)
							{
								objInst.data->DecRefHandle();
								objInst.data = nullptr;
							}
							HUDMenu * pHUD = nullptr;
							if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
							{
								GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;

								GFxValue params[2];
								movieRoot->CreateArray(&params[0]);
								movieRoot->CreateObject(&params[1]);
								OnCrosshairRefChange(movieRoot, &params[0], &params[1], pForm, pStack->extraData, pStack->count);
								movieRoot->Invoke("root.HUDPlusPlusLoader.content.onQuickContainerEnter", nullptr, params, 2);
							}
						}
					}
					pRef->handleRefObject.DecRefHandle();
					RelocAddr<void(*)(bool[])>  DecTLSRef = 0xEC01D0;
					DecTLSRef(tlsState);
				}
			}
			else
			{
				HUDMenu * pHUD = nullptr;
				if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
				{
					GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;
					movieRoot->Invoke("root.HUDPlusPlusLoader.content.onQuickContainerLeave", nullptr, nullptr, 0);
				}
			}
		}
		(this->*fnUpdateElement)();
	}

	static void InitHook()
	{
		fnUpdateElement = HookUtil::SafeWrite64(RelocAddr<uintptr_t>(0x2D2E518) + 4 * 0x8, &UpdateElement_Hook);
	}
};
HUDQuickContainerEx::FnUpdateElement		HUDQuickContainerEx::fnUpdateElement = nullptr;


class HUDQuickContainerDataModelEx : public HUDQuickContainerDataModel
{
public:
	using FnOnButtonEvent		= void(__thiscall HUDQuickContainerDataModelEx::*)(ButtonEvent *);

	static FnOnButtonEvent		fnOnButtonEvent;

	void OnButtonEvent_Hook(ButtonEvent * inputEvent)
	{
		if ((*inputEvent->GetControlID()) == "QCTakeItem" && isPickUpKeyDown && inputEvent->IsUp())
		{
			SimpleLocker locker(&g_quickContainerLock);
			(this->*fnOnButtonEvent)(inputEvent);
			return;
		}
		(this->*fnOnButtonEvent)(inputEvent);
	}

	static void InitHook()
	{
		fnOnButtonEvent = HookUtil::SafeWrite64(RelocAddr<uintptr_t>(0x2D2EA78) + 0x8 * 8, &OnButtonEvent_Hook);
	}
};
HUDQuickContainerDataModelEx::FnOnButtonEvent	HUDQuickContainerDataModelEx::fnOnButtonEvent = nullptr;


class InputControl : public BSInputEventUser
{
public:
	InputControl() : BSInputEventUser(true) {}
	virtual ~InputControl() {}

	virtual bool IsEnabled(InputEvent * inputEvent) override
	{
		if (inputEvent->eventType == InputEvent::kEventType_Button)
			return true;
		//_MESSAGE("controlID: %s | %d", inputEvent->GetControlID()->c_str(), inputEvent->eventType);
		return false;
	};

	virtual void OnButtonEvent(ButtonEvent * inputEvent) override
	{
		UInt32	keyCode;
		UInt32	deviceType = inputEvent->deviceType;
		UInt32	keyMask = inputEvent->keyMask;

		if ((*g_ui) && (*g_ui)->menuMode)
			return;
		// Mouse
		if (deviceType == InputEvent::kDeviceType_Mouse)
			keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
		// Gamepad
		else if (deviceType == InputEvent::kDeviceType_Gamepad)
			keyCode = InputMap::GamepadMaskToKeycode(keyMask);
		// Keyboard
		else
			keyCode = keyMask;
		//ESSAGE("keyCode=%d", keyCode);
		if (keyCode == ModSettings::iQuickUseHotkey && inputEvent->IsDown())
		{
			if ((*g_HUDDataModel) != nullptr)
			{
				auto pQC = (*g_HUDDataModel)->GetQCData();
				TESObjectREFR * pRef = nullptr;
				LookupREFRByHandle(&pQC->refHandle, &pRef);
				if (pRef != nullptr)
				{
					SimpleLocker locker(&g_quickContainerLock);
					TESForm * pForm = nullptr;
					BGSInventoryItem::Stack * pStack = nullptr;
					bool tlsState[2];
					RelocAddr<void(*)(bool[], bool, bool)>  IncTLSRef = 0xEC0180;
					IncTLSRef(tlsState, true, false);

					if ((pQC->type == 0 || pQC->type == 6) && pQC->dataLen && pQC->selectedIndex >= 0\
						&& !pQC->unk228 && pQC->selectedIndex < pQC->dataLen)
					{
						auto pEntry = &pQC->entry[pQC->selectedIndex];
						RelocAddr<TESForm*(*)(uintptr_t, HUDQuickContainerDataModel::Entry*)>	GetSelectedForm = 0x1A3740;
						pForm = GetSelectedForm((*RelocPtr<uintptr_t>(0x58F2D80)), pEntry);
						if (pForm != nullptr)
						{
							RelocAddr<BGSInventoryItem *(*)(uintptr_t, HUDQuickContainerDataModel::Entry*)>	GetSelectedInventoryItem = 0x1A3650;
							auto pItem = GetSelectedInventoryItem((*RelocPtr<uintptr_t>(0x58F2D80)), pEntry);
							SInt16 stackIndex = (pEntry->flags & 0x80000000) ? pEntry->stackIndex.index : (*pEntry->stackIndex.indexs);
							auto GetStack = [](BGSInventoryItem * pItem, SInt16 index)->BGSInventoryItem::Stack *
							{
								if (!pItem->stack)	return nullptr;
								auto pStack = pItem->stack;
								while (index-- && (pStack = pStack->next, pStack)) {}
								return pStack;
							};
							pStack = GetStack(pItem, stackIndex);
						}
					}
					if (pForm != nullptr && pForm->formType == kFormType_NOTE)
					{
						auto pNote = static_cast<BGSNote*>(pForm);
						if (pNote->holotapeType == BGSNote::kNote_SECN \
							|| pNote->holotapeType > 3)
						{
							//need to check whether activate is disabled...
							(*g_player)->StopPlayHolotape(false);
							(*g_player)->note = pNote;
							(*g_player)->PlayHolotape(pNote); //without animation.
						}
						else if ((*g_pipboyManager) != nullptr)
						{
							(*g_player)->StopPlayHolotape(false);
							(*g_player)->note = pNote;
							(*g_pipboyManager)->PlayHolotape(pNote, false); //with animation.
						}
						//pQC->PickUpItem(); //mutex lock.
					}
					if (pForm && pForm->formType == kFormType_BOOK && pStack && pStack->extraData)
					{
						BSString str;
						auto pBook = static_cast<TESObjectBOOK*>(pForm);
						if (pBook->flags & TESObjectBOOK::kType_Read || !(pBook->flags & TESObjectBOOK::kType_AddPerk))
						{
							auto pDesc = &(pBook->description);
							pDesc->Get(str, nullptr);
							NiNode* pNode = (*g_player)->GetObjectRootNode();
							DisplayBookMenu(str, pStack->extraData, nullptr, pBook, pNode->m_worldTransform.pos, pNode->m_worldTransform.rot, pNode->m_localTransform.scale, false);
						}
					}
					RelocAddr<void(*)(bool[])>  DecTLSRef = 0xEC01D0;
					DecTLSRef(tlsState);
					pRef->handleRefObject.DecRefHandle();
					pQC->PickUpItem();
				}
			}
		}
	};

	static void Register()
	{
		if (!(*g_menuControls))
		{
			_MESSAGE("failed to find menu input controller...");
			return;
		}
		static BSInputEventUser * pInputHandler = new InputControl();
		tArray<BSInputEventUser*>& inputEvents = (*g_menuControls)->inputEvents;
		SInt64 index = inputEvents.GetItemIndex(pInputHandler);
		if (index == -1)
		{
			inputEvents.Push(pInputHandler);
		}
	}
};


class CrosshairChangeHandler : public BSTEventSink<ViewCasterUpdateEvent>
{
	//80 7F 40 00 0F 84 8A 00 00 00 80 7F 38 00 8B 47 08  //sig
public:
	virtual ~CrosshairChangeHandler() { };
	virtual	EventResult	ReceiveEvent(ViewCasterUpdateEvent * evn, void * dispatcher) override
	{
		TESObjectREFR* pRef = nullptr;
		LookupREFRByHandle(&evn->crosshairHandler, &pRef);

		HUDRolloverEx::pCrosshairRef = pRef;
		if (pRef != nullptr)
		{
			pRef->handleRefObject.DecRefHandle();
		}
		return kEvent_Continue;
	}

	static void Register()
	{
		RelocPtr<BSTEventDispatcher<ViewCasterUpdateEvent> *> pDispatcher = 0x5A7DAE8;///V1.10.26
		static auto pCrosshairHandler = new CrosshairChangeHandler();
		if (*pDispatcher != nullptr)
		{
			reinterpret_cast<BSTEventDispatcher<ViewCasterUpdateEvent> *>((uintptr_t)(*pDispatcher) + 0x10)->AddEventSink(pCrosshairHandler);
		}
		else
		{
			RelocAddr<BSTEventDispatcher<ViewCasterUpdateEvent> *(*)(uintptr_t, uintptr_t)> GetCrosshairEventDispatcher = 0x9E3230;		///V1.10.26
			auto pDispatcher = GetCrosshairEventDispatcher(RelocAddr<uintptr_t>(0x5A7DB60).GetUIntPtr(), (*RelocAddr<uintptr_t*>(0x58ED018)) + 0x10); ///V1.10.26 TESGlobalEventSource
			if (pDispatcher != nullptr)
			{
				reinterpret_cast<BSTEventDispatcher<ViewCasterUpdateEvent> *>((uintptr_t)pDispatcher + 0x10)->AddEventSink(pCrosshairHandler);
			}
			_MESSAGE("Can't register crosshair update handler...");
		}
	}
};


class HUDModeEventHandler : public BSTEventSink<HUDModeEvent>
{
public:
	virtual ~HUDModeEventHandler() { };
	virtual	EventResult	ReceiveEvent(HUDModeEvent * evn, void * dispatcher) override
	{
		static BSFixedString powerArmorMode("PowerArmor");
		static BSFixedString menuName("HUDMenu");
		bool result = false;
		for (size_t i = 0; i < evn->modes->count; ++i)
		{
			if (evn->modes->operator[](i) == powerArmorMode)
			{
				result = true;
			}
			//_MESSAGE("mode=%s", evn->modes->operator[](i).c_str());
		}
		//_MESSAGE(" ");
		isInPAState = (result) ? true : false;
		HUDMenu * pHUD = nullptr;
		if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
		{
			// in current version we don't need these...
			/*
			GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;
			GFxValue HUDPlusPlus;
			if (movieRoot->Invoke("root.CenterGroup_mc.getChildByName", &HUDPlusPlus, &GFxValue("HUDPlusPlus"), 1))
			{
			GFxValue param;
			param.SetBool(isInPAState);
			HUDPlusPlus.Invoke("SetPowerArmorState", nullptr, &param, 1);
			}
			*/
		}
		return kEvent_Continue;
	}

	static void Register()
	{
		if ((*g_HUDModeEventSource) != nullptr)
		{
			static auto pHandler = new HUDModeEventHandler;
			(*g_HUDModeEventSource)->source.AddEventSink(pHandler);
		}
	}

private:
	bool					isInPAState;
};


class TESLoadGameHandler : public BSTEventSink<TESLoadGameEvent>
{
public:
	virtual ~TESLoadGameHandler() { };
	virtual	EventResult	ReceiveEvent(TESLoadGameEvent * evn, void * dispatcher) override
	{
		CrosshairChangeHandler::Register();
		HUDModeEventHandler::Register();
		return kEvent_Continue;
	}
};


class HUDPlusPlus_WriteLog : public GFxFunctionHandler//test only...
{
public:
	virtual void Invoke(Args * args) override
	{
		ASSERT(args->numArgs >= 1);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);
		_MESSAGE(args->args[0].GetString());
	}
};

class HUDPlusPlus_OnModSettingChange : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args) override
	{
		/*
		std::string identifier = args->args[0].GetString();
		auto delimiter = identifier.find('|');
		if (delimiter != std::string::npos)
		{
			std::string settingName = identifier.substr(0, delimiter);
			std::string settingValue = identifier.substr(delimiter + 1);
			UInt32 result = std::stoll(settingValue, nullptr, 10);
			if (settingName == "iDisplayItemCard")
			{
				ModSettings::iDisplayItemCard = result;
			}
		}
		*/	
		ModSettings::ReadModSettings();
		HUDMenu * pHUD = nullptr;
		static BSFixedString menuName("HUDMenu");
		if ((*g_ui) != nullptr && (pHUD = static_cast<HUDMenu*>((*g_ui)->GetMenu(&menuName)), pHUD))
		{
			GFxMovieRoot * movieRoot = pHUD->movie->movieRoot;
			GFxValue settings;
			movieRoot->CreateObject(&settings);
			Register<bool>(&settings, "bShowCrosshair", ModSettings::bShowCrosshair);
			Register<bool>(&settings, "bSpecialItemIconVisible", ModSettings::bSpecialItemIconVisible);
			Register<bool>(&settings, "bCrosshairItemCardVisible", ModSettings::bCrosshairItemCardVisible);
			Register<bool>(&settings, "bQuickContainerItemCardVisible", ModSettings::bQuickContainerItemCardVisible);
			Register<bool>(&settings, "bBriefItemInfoWidgetVisible", ModSettings::bBriefItemInfoWidgetVisible);

			Register<UInt32>(&settings, "iCrosshairItemCardCoordX", ModSettings::iCrosshairItemCardCoordX);
			Register<UInt32>(&settings, "iCrosshairItemCardCoordY", ModSettings::iCrosshairItemCardCoordY);
			Register<UInt32>(&settings, "iQuickContainerItemCardCoordX", ModSettings::iQuickContainerItemCardCoordX);
			Register<UInt32>(&settings, "iBriefItemInfoWidgetCoordX", ModSettings::iBriefItemInfoWidgetCoordX);
			Register<UInt32>(&settings, "iBriefItemInfoWidgetCoordY", ModSettings::iBriefItemInfoWidgetCoordY);

			Register<double>(&settings, "fCrosshairItemCardAlpha", ModSettings::fCrosshairItemCardAlpha);
			Register<double>(&settings, "fCrosshairItemCardScale", ModSettings::fCrosshairItemCardScale);
			Register<double>(&settings, "fQuickContainerItemCardAlpha", ModSettings::fQuickContainerItemCardAlpha);
			Register<double>(&settings, "fQuickContainerItemCardScale", ModSettings::fQuickContainerItemCardScale);
			Register<double>(&settings, "fBriefItemInfoWidgetScale", ModSettings::fBriefItemInfoWidgetScale);

			movieRoot->Invoke("root.HUDPlusPlusLoader.content.onModSettingChange", nullptr, &settings, 1);
		}
	}
};

class HUDPlusPlus_GetModSettings : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args) override
	{
		GFxMovieRoot * movieRoot = args->movie->movieRoot;
		movieRoot->CreateObject(args->result);
		Register<bool>(args->result, "bShowCrosshair", ModSettings::bShowCrosshair);
		Register<bool>(args->result, "bSpecialItemIconVisible", ModSettings::bSpecialItemIconVisible);
		Register<bool>(args->result, "bCrosshairItemCardVisible", ModSettings::bCrosshairItemCardVisible);
		Register<bool>(args->result, "bQuickContainerItemCardVisible", ModSettings::bQuickContainerItemCardVisible);
		Register<bool>(args->result, "bBriefItemInfoWidgetVisible", ModSettings::bBriefItemInfoWidgetVisible);

		Register<UInt32>(args->result, "iCrosshairItemCardCoordX", ModSettings::iCrosshairItemCardCoordX);
		Register<UInt32>(args->result, "iCrosshairItemCardCoordY", ModSettings::iCrosshairItemCardCoordY);
		Register<UInt32>(args->result, "iQuickContainerItemCardCoordX", ModSettings::iQuickContainerItemCardCoordX);
		Register<UInt32>(args->result, "iBriefItemInfoWidgetCoordX", ModSettings::iBriefItemInfoWidgetCoordX);
		Register<UInt32>(args->result, "iBriefItemInfoWidgetCoordY", ModSettings::iBriefItemInfoWidgetCoordY);

		Register<double>(args->result, "fCrosshairItemCardAlpha", ModSettings::fCrosshairItemCardAlpha);
		Register<double>(args->result, "fCrosshairItemCardScale", ModSettings::fCrosshairItemCardScale);
		Register<double>(args->result, "fQuickContainerItemCardAlpha", ModSettings::fQuickContainerItemCardAlpha);
		Register<double>(args->result, "fQuickContainerItemCardScale", ModSettings::fQuickContainerItemCardScale);
		Register<double>(args->result, "fBriefItemInfoWidgetScale", ModSettings::fBriefItemInfoWidgetScale);
	}
};

class HUDPlusPlus_IsInPowerArmorMode : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args) override
	{
		if ((*g_player) != nullptr)
		{
			args->result->SetBool((*g_player)->IsInPowerArmorMode());
		}
		else
		{
			args->result->SetBool(false);
		}
	}
};


class HUDPlusPlus_OnMCMKeybindChange : public GFxFunctionHandler
{
public:
	using FnInvoke = void(__thiscall HUDPlusPlus_OnMCMKeybindChange::*)(Args *);
	using FnCreateFunction = void(*)(GFxMovieRoot *, GFxValue *, GFxFunctionHandler *, void *);

	static FnCreateFunction fnCreateFunction;
	static FnInvoke			MCM_ClearKeybind;
	static FnInvoke			MCM_RemapKeybind;
	static FnInvoke			MCM_SetKeybind;

	virtual void Invoke(Args * args) override {}

	void ClearKeybind(Args * args)
	{
		if (args->numArgs >= 2 && ((args->args[0].GetType() == GFxValue::kType_String\
			&& args->args[1].GetType() == GFxValue::kType_String \
			&& strcmp(args->args[0].GetString(), "HUDPlusPlus") == 0\
			&& strcmp(args->args[1].GetString(), "HUDPlusPlus_SetQuickUseHotkey") == 0) \
			|| (args->args[0].GetType() == GFxValue::kType_Int \
			&& args->args[1].GetType() == GFxValue::kType_Int \
			&& args->args[0].GetInt() == ModSettings::iQuickUseHotkey)))
		{
			ModSettings::iQuickUseHotkey = 0;
			ModSettings::CommitModSettings("Settings", "iQuickUseHotkey", 0);
		}
		(this->*MCM_ClearKeybind)(args);
	}

	void RemapKeybind(Args * args)
	{
		if (args->numArgs >= 4 && args->args[0].GetType() == GFxValue::kType_String \
			&& args->args[1].GetType() == GFxValue::kType_String\
			&& args->args[2].GetType() == GFxValue::kType_Int \
			&& args->args[3].GetType() == GFxValue::kType_Int \
			&& strcmp(args->args[0].GetString(), "HUDPlusPlus") == 0\
			&& strcmp(args->args[1].GetString(), "HUDPlusPlus_SetQuickUseHotkey") == 0)
		{
			ModSettings::iQuickUseHotkey = args->args[2].GetInt();
			ModSettings::CommitModSettings("Settings", "iQuickUseHotkey", args->args[2].GetInt());
		}
		(this->*MCM_RemapKeybind)(args);
	}

	void SetKeybind(Args * args)
	{
		if (args->numArgs >= 4 && args->args[0].GetType() == GFxValue::kType_String \
			&& args->args[1].GetType() == GFxValue::kType_String\
			&& args->args[2].GetType() == GFxValue::kType_Int \
			&& args->args[3].GetType() == GFxValue::kType_Int \
			&& strcmp(args->args[0].GetString(), "HUDPlusPlus") == 0\
			&& strcmp(args->args[1].GetString(), "HUDPlusPlus_SetQuickUseHotkey") == 0)
		{
			ModSettings::iQuickUseHotkey = args->args[2].GetInt();
			ModSettings::CommitModSettings("Settings", "iQuickUseHotkey", args->args[2].GetInt());
		}
		(this->*MCM_SetKeybind)(args);
	}

	static void CreateFunction_Hook(GFxMovieRoot * pRoot, GFxValue* pValue, GFxFunctionHandler* pCallback, void* puserData)
	{
		if (pCallback != nullptr)
		{
			constexpr char* sNameA = "class ScaleformMCM::SetKeybind";
			constexpr char* sNameB = "class ScaleformMCM::ClearKeybind";
			constexpr char* sNameC = "class ScaleformMCM::RemapKeybind";

			static bool bSkipA = false;
			//_MESSAGE("className: %s  VTBL: %016I64X  FN: %016I64X", typeid(*pCallback).name(), *(uintptr_t*)pCallback, *(uintptr_t*)(*(uintptr_t*)pCallback + 8));
			if (!bSkipA && strcmp(sNameA, typeid(*pCallback).name()) == 0)
			{
				MCM_SetKeybind = HookUtil::SafeWrite64((uintptr_t)(*(uintptr_t**)pCallback + 0x1), &SetKeybind);
				bSkipA = true;
			}
			static bool bSkipB = false;
			if (!bSkipB && strcmp(sNameB, typeid(*pCallback).name()) == 0)
			{
				MCM_ClearKeybind = HookUtil::SafeWrite64((uintptr_t)(*(uintptr_t**)pCallback + 0x1), &ClearKeybind);
				bSkipB = true;
			}
			static bool bSkipC = false;
			if (!bSkipC && strcmp(sNameC, typeid(*pCallback).name()) == 0)
			{
				MCM_RemapKeybind = HookUtil::SafeWrite64((uintptr_t)(*(uintptr_t**)pCallback + 0x1), &RemapKeybind);
				bSkipC = true;
			}
			//_MESSAGE("pFunctionHandle: %016I64X  sName: %s", *(uintptr_t*)pCallback, typeid(*pCallback).name());
		}
		fnCreateFunction(pRoot, pValue, pCallback, puserData);
	}

	static void InitHooks()
	{
		fnCreateFunction = HookUtil::SafeWrite64(RelocAddr<uintptr_t>(0x2EC87E0).GetUIntPtr() + 8 * 0x30, CreateFunction_Hook); //V1.10.26   48 85 C0 0F 84 37 01 00 00 48 8B 59 48 48 85 DB 0F 84 2A 01 00 00
	}
};
HUDPlusPlus_OnMCMKeybindChange::FnCreateFunction	HUDPlusPlus_OnMCMKeybindChange::fnCreateFunction = nullptr;
HUDPlusPlus_OnMCMKeybindChange::FnInvoke			HUDPlusPlus_OnMCMKeybindChange::MCM_SetKeybind = nullptr;
HUDPlusPlus_OnMCMKeybindChange::FnInvoke			HUDPlusPlus_OnMCMKeybindChange::MCM_ClearKeybind = nullptr;
HUDPlusPlus_OnMCMKeybindChange::FnInvoke			HUDPlusPlus_OnMCMKeybindChange::MCM_RemapKeybind = nullptr;


void MessageCallback(F4SEMessagingInterface::Message * msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_GameDataReady)
	{
		static auto pHandler = new TESLoadGameHandler();
		GetEventDispatcher<TESLoadGameEvent>()->AddEventSink(pHandler);

		InputControl::Register();

		BSScaleformTranslator * pTranslator = reinterpret_cast<BSScaleformTranslator*>((*g_scaleformManager)->stateBag->GetStateAddRef(GFxState::kInterface_Translator));
		if (pTranslator != nullptr) 
		{
			Translation::ParseTranslation(pTranslator, std::string("HUDPlusPlus"));
		}
		//for (size_t i = 0; i < (*g_dataHandler)->arrNOTE.count; ++i)
		//{
		//	_MESSAGE(">>>> %08X | %s | %d", (*g_dataHandler)->arrNOTE[i]->formID, (*g_dataHandler)->arrNOTE[i]->swfName.c_str(), (*g_dataHandler)->arrNOTE[i]->holotapeType);
		//	DumpClass((*g_dataHandler)->arrNOTE[i], 0x108 >> 3);
		//}
	}
}


bool ScaleformCallback(GFxMovieView * view, GFxValue * value)
{
	RegisterFunction<HUDPlusPlus_WriteLog>(value, view->movieRoot, "log");
	RegisterFunction<HUDPlusPlus_OnModSettingChange>(value, view->movieRoot, "onModSettingChange");
	RegisterFunction<HUDPlusPlus_IsInPowerArmorMode>(value, view->movieRoot, "isInPowerArmorMode");
	RegisterFunction<HUDPlusPlus_GetModSettings>(value, view->movieRoot, "getModSettings");
	//HUDPlusPlus_ShowCrosshair
	GFxMovieRoot * movieRoot = view->movieRoot;
	if (movieRoot)
	{
		GFxValue result;
		GFxValue stage;
		movieRoot->GetVariable(&stage, "stage");
		GFxValue firstChild;
		stage.Invoke("getChildAt", &firstChild, &GFxValue((SInt32)0), 1);
		movieRoot->Invoke("flash.utils.getQualifiedClassName", &result, &firstChild, 1);
		if (result.IsString())
		{
			const char * clipName = result.GetString();
			if (strcmp("HUDMenu", clipName) == 0)
			{
				HUDRolloverEx::pGFxMovieView = view;
				GFxValue centerGroup;
				if (movieRoot->GetVariable(&centerGroup, "root.CenterGroup_mc"))
				{
					_MESSAGE("%s >> found swf path...", __FUNCTION__);
					GFxValue root;
					movieRoot->GetVariable(&root, "root");
					GFxValue loader;
					movieRoot->CreateObject(&loader, "flash.display.Loader");
					root.SetMember("HUDPlusPlusLoader", &loader);

					GFxValue loadArgs[2];
					movieRoot->CreateObject(&loadArgs[0], "flash.net.URLRequest", &GFxValue("HUDPlusPlus.swf"), 1);
					loadArgs[1].SetNull();
					movieRoot->Invoke("root.addChild", nullptr, &loader, 1);
					if (!loader.Invoke("load", nullptr, loadArgs, 2))
					{
						_MESSAGE("%s >> failed to inject flash widget...", __FUNCTION__);
					}
				}
			}
		}
	}
	return true;
}

#ifdef _DEBUG
void InitRVA()
{
	g_scaleformManager				= RVA<BSScaleformManager**>(GET_RVA(g_scaleformManager), "48 8B 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 8B D3", 0, 3, 7);
	g_menuControls					= RVA<MenuControls**>(GET_RVA(g_menuControls), "48 8B 0D ? ? ? ? E8 ? ? ? ? 80 3D ? ? ? ? ? 0F B6 F8", 0, 3, 7);
	g_ui							= RVA<UI**>(GET_RVA(g_ui), "48 8B 0D ? ? ? ? BA ? ? ? ? 8B 1C 16", 0, 3, 7);
	g_inputMgr						= RVA<InputManager**>(GET_RVA(g_inputMgr), "48 83 3D ? ? ? ? ? 74 3F 48 83 C1 40", 0, 3, 8);
	g_dataHandler					= RVA<DataHandler**>(GET_RVA(g_dataHandler), "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7);
	g_HUDQuickContainerVTBL			= RVA<void *>(GET_RVA(g_HUDQuickContainerVTBL), "48 8D 05 ? ? ? ? 48 8B D9 48 89 01 48 8D 05 ? ? ? ? 48 89 41 50 48 8D 05 ? ? ? ? 48 89 81 E0 07 00 00", 0, 3, 7);
	g_viewCasterUpdateEventSource	= RVA<ViewCasterUpdateEventSource **>(GET_RVA(g_viewCasterUpdateEventSource), "48 8B 9C 24 88 00 00 00 48 85 DB 0F 84 82 01 00 00 F3 0F 10 47 40 0F 57 C9 48 89 74 24 70 0F 2E  C1 75 05 8D 45 FF EB 03", -0x32, 3, 7);
	g_HUDDataModel					= RVA<HUDDataModel **>(GET_RVA(g_HUDDataModel), "0F 57 C0 66 0F 7F 45 C0 48 8D 45 A8 48 8D 9F 98 13 00 00 33 D2 48 8B CB 48 89 75 A8 48 89 75 B8", -7, 3, 7);
	g_pipboyManager					= RVA<PipboyManager **>(GET_RVA(g_pipboyManager), "84 C0 74 1B 80 B9 C6 01 00 00 00 75 12 80 B9 C7 01 00 00 00 75 09 83 B9 A8 01 00 00 00 74 15", -7, 3, 7);
	g_HUDModeEventSource			= RVA<HUDModeEventSource **>(GET_RVA(g_HUDModeEventSource), "48 8B 0D ? ? ? ? 48 85 C9 74 0C 48 83 C1 10 49 8B D6 E8 ? ? ? ? 49 8B 5E 38 48 85 DB 74  39 41 8B 46 48 85 C0 74 20", 0, 3, 7);


	CalcInstanceData				= RVA<_CalcInstanceData>(GET_RVA(CalcInstanceData), "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 89 11 49 8B F8 48 8B F2 4C 89 41 08 48 8B D9 4D  85 C0 74 05 F0 41 FF 40 08 48 85 D2 74 59 48 8B 02 48 8B CA FF 90 98 01 00 00");
	DisplayBookMenu					= RVA<_DisplayBookMenu>(GET_RVA(DisplayBookMenu), "49 8B F0 43 8B 1C 2F 43 C7 04 2F 42 00 00 00 48 8B 01 48 85 C0", -0x2E);
}
#endif

void InitHooks()
{
	ModSettings::ReadModSettings();
	if (!g_branchTrampoline.Create(1024 * 64))
	{
		_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
		return;
	}
	HUDRolloverEx::InitHook();
	HUDQuickContainerEx::InitHook();
	HUDQuickContainerDataModelEx::InitHook();
	HUDPlusPlus_OnMCMKeybindChange::InitHooks();
	//Crash fix
	SafeWrite8(RelocAddr<uintptr_t>(0x1153A0).GetUIntPtr(), 0xC9);

	UInt8 instructions[] = { 0xB0, 0x01, 0xC3 };
	SafeWriteBuf(RelocAddr<uintptr_t>(0xCD8D80).GetUIntPtr(), instructions, sizeof(instructions));
}


