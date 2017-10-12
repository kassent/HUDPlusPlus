#include "Defination.h"
RelocAddr<_DisplayBookMenu>				DisplayBookMenu(0x12566D0);
RelocAddr<_CalcInstanceData>			CalcInstanceData(0x2F7A30); ///V1.10.26

//RelocAddr<HUDDataModel *>				hudDataModel(0x5A812E0);
RelocPtr<HUDDataModel *>				g_HUDDataModel(0x5A7EDC0);
RelocPtr<HUDModeEventSource *>			g_HUDModeEventSource(0x5A7F200); //RelocAddr 5A7F220

RelocPtr<ViewCasterUpdateEventSource *>	g_viewCasterUpdateEventSource(0x5900F00);

RelocPtr<PipboyManager *>				g_pipboyManager(0x59F81A0);

RelocAddr<_PlayHolotape>				PlayHolotape = 0xB8EC70; // called by pipboy menu.

//RelocAddr<HUDQuickContainerDataModel *> g_qucikContainerDataModel(0x5A81318); //5A81318

RelocAddr<void *>						g_HUDQuickContainerVTBL(0x2D2E518);