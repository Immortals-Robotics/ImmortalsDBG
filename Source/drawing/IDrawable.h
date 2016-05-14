#pragma once
#include "../../3rdparty/imgui/imgui.h"

class IDrawable
{
public:
	virtual void draw(ImDrawList* draw_list, float zoom, ImVec2 margin) = 0;
};