#include "ModSettings.h"
#include <memory>
#include <string>
namespace ModSettings
{
	bool		bShowCrosshair					= true;
	bool		bSpecialItemIconVisible			= true;
	bool		bCrosshairItemCardVisible		= true;
	bool		bQuickContainerItemCardVisible	= true;
	bool		bBriefItemInfoWidgetVisible		= true;

	UInt32		iQuickUseHotkey					= 70;
	UInt32		iCrosshairItemCardCoordX		= 640;
	UInt32		iCrosshairItemCardCoordY		= 320;
	UInt32		iQuickContainerItemCardCoordX	= 640;
	UInt32		iBriefItemInfoWidgetCoordX		= 640;
	UInt32		iBriefItemInfoWidgetCoordY		= 320;

	float		fCrosshairItemCardAlpha			= 1.0f;
	float		fCrosshairItemCardScale			= 0.7f;
	float		fQuickContainerItemCardAlpha	= 1.0f;
	float		fQuickContainerItemCardScale	= 0.7f;
	float		fBriefItemInfoWidgetScale		= 1.0f;

	void ReadModSettings()
	{
		constexpr char * iniPath = ".\\Data\\MCM\\Settings\\HUDPlusPlus.ini";
		constexpr char * sectionName = "Settings";
		std::unique_ptr<char[]> sResult(new char[MAX_PATH]);

		bShowCrosshair					 = GetPrivateProfileIntA(sectionName, "bShowCrosshair", 1, iniPath) != 0;
		bSpecialItemIconVisible			 = GetPrivateProfileIntA(sectionName, "bSpecialItemIconVisible", 1, iniPath) != 0;
		bCrosshairItemCardVisible		 = GetPrivateProfileIntA(sectionName, "bCrosshairItemCardVisible", 1, iniPath) != 0;
		bQuickContainerItemCardVisible	 = GetPrivateProfileIntA(sectionName, "bQuickContainerItemCardVisible", 1, iniPath) != 0;
		bBriefItemInfoWidgetVisible		 = GetPrivateProfileIntA(sectionName, "bBriefItemInfoWidgetVisible", 1, iniPath) != 0;
		iQuickUseHotkey					 = GetPrivateProfileIntA(sectionName, "iQuickUseHotkey", 0, iniPath);
		iCrosshairItemCardCoordX		 = GetPrivateProfileIntA(sectionName, "iCrosshairItemCardCoordX", 640, iniPath);
		iCrosshairItemCardCoordY		 = GetPrivateProfileIntA(sectionName, "iCrosshairItemCardCoordY", 320, iniPath);
		iQuickContainerItemCardCoordX	 = GetPrivateProfileIntA(sectionName, "iQuickContainerItemCardCoordX", 640, iniPath);
		iBriefItemInfoWidgetCoordX		 = GetPrivateProfileIntA(sectionName, "iBriefItemInfoWidgetCoordX", 640, iniPath);
		iBriefItemInfoWidgetCoordY		 = GetPrivateProfileIntA(sectionName, "iBriefItemInfoWidgetCoordY", 320, iniPath);

		GetPrivateProfileStringA(sectionName, "fCrosshairItemCardScale", "0.7", sResult.get(), MAX_PATH, iniPath);
		fCrosshairItemCardScale = std::stof(sResult.get());

		GetPrivateProfileStringA(sectionName, "fQuickContainerItemCardScale", "0.7", sResult.get(), MAX_PATH, iniPath);
		fQuickContainerItemCardScale = std::stof(sResult.get());

		GetPrivateProfileStringA(sectionName, "fBriefItemInfoWidgetScale", "1.0", sResult.get(), MAX_PATH, iniPath);
		fBriefItemInfoWidgetScale = std::stof(sResult.get());
	}
}