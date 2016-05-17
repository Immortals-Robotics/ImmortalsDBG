#pragma once

#include "../../3rdparty/imgui/imgui.h"
#include "colors.h"
#include "vectors.h"

#ifndef IM_PI
#define IM_PI					3.14159265358979323846f
#endif // !
#ifndef IM_2PI
#define IM_2PI					6.28318530717958647692f
#endif // !






void AddSegmetFilled(ImDrawList* x, const ImVec2& centre, float radius, ImU32 col, ImVec2 radian, int num_segments)
{
	if ((col >> 24) == 0)
		return;

	const ImVec2 a_max = (radian);// *((float)num_segments - 1.0f) / (float)num_segments;
	x->PathArcTo(centre, radius, a_max.x, a_max.y, num_segments);
	//x->PathFill(col);
	x->PathStroke(col, false, 1.f);
}


enum Team {
	Blue = 0,
	Yellow = 1,
};