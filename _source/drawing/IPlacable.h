#pragma once
#include "../../3rdparty/imgui/imgui.h"

__interface IPlacable
{
	void put(ImVec2 pos);
	ImVec2 whereIs();
	bool DoesOccupy(ImVec2 point);
};
