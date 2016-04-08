#pragma once
#include "../../3rdparty/imgui/imgui.h"

__interface IDrawable
{
public:
	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin);
};