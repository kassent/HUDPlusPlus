#pragma once
#include <string>
namespace ModSettings
{
	extern		bool		bShowCrosshair;
	extern		bool		bSpecialItemIconVisible;
	extern		bool		bCrosshairItemCardVisible;
	extern		bool		bQuickContainerItemCardVisible;
	extern		bool		bBriefItemInfoWidgetVisible;
	extern		UInt32		iQuickUseHotkey;
	extern		UInt32		iCrosshairItemCardCoordX;
	extern		UInt32		iCrosshairItemCardCoordY;
	extern		UInt32		iQuickContainerItemCardCoordX;
	extern		UInt32		iBriefItemInfoWidgetCoordX;
	extern		UInt32		iBriefItemInfoWidgetCoordY;
	extern		float		fCrosshairItemCardAlpha;
	extern		float		fCrosshairItemCardScale;
	extern		float		fQuickContainerItemCardAlpha;
	extern		float		fQuickContainerItemCardScale;
	extern		float		fBriefItemInfoWidgetScale;

	template<typename T>
	inline void CommitModSettings(const char * sectionName, const char * settingName, T value) //bool
	{
		constexpr char * iniPath = ".\\Data\\MCM\\Settings\\HUDPlusPlus.ini";
		if (GetFileAttributesA("Data\\MCM\\Settings") != INVALID_FILE_ATTRIBUTES)
		{
			WritePrivateProfileStringA(sectionName, settingName, std::to_string(value).c_str(), iniPath);
		}
	}

	template<>
	inline void CommitModSettings(const char * sectionName, const char * settingName, const char * value)
	{
		constexpr char * iniPath = ".\\Data\\MCM\\Settings\\HUDPlusPlus.ini";
		if (GetFileAttributesA("Data\\MCM\\Settings") != INVALID_FILE_ATTRIBUTES)
		{
			WritePrivateProfileStringA(sectionName, settingName, value, iniPath);
		}
	}

	void ReadModSettings();
}
