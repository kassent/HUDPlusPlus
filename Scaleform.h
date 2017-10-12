#pragma once
#include "F4SE/PluginAPI.h"

class GFxValue;
class GFxMovieView;

bool	ScaleformCallback(GFxMovieView * view, GFxValue * value);
void	MessageCallback(F4SEMessagingInterface::Message* msg);
void	InitHooks();

