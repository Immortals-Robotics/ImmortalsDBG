#include "scaled_imdrawlist.h"
#include "vector_helper.h"

ImDrawListScaled::ImDrawListScaled()
{
	offset = ImVec2(0.f, 0.f);
	scale = 1.f;
}

void ImDrawListScaled::AddLineScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
	drawList->AddLine(offset + a * scale , offset + b * scale, col, thickness);
}

void  ImDrawListScaled::AddRectScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners)
{
	drawList->AddRect(offset + a * scale , offset + b * scale, col, rounding, rounding_corners);
}

void  ImDrawListScaled::AddRectFilledScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners)
{
	drawList->AddRectFilled(offset + a * scale , offset + b * scale, col, rounding, rounding_corners);
}

void  ImDrawListScaled::AddCircleScaled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
	drawList->AddCircle(offset + centre * scale, radius * scale, col, num_segments);
}

void  ImDrawListScaled::AddCircleFilledScaled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
	drawList->AddCircleFilled(offset + centre * scale, radius * scale, col, num_segments);
}

void  ImDrawListScaled::AddTextScaled(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
	drawList->AddText(offset + pos * scale, col, text_begin, text_end);
}

void ImDrawListScaled::AddSegmetScaled(const ImVec2& centre, float radius, ImU32 col, ImVec2 radian, int num_segments)
{
	if ((col >> 24) == 0)
		return;

	const ImVec2 a_max = (radian);// *((float)num_segments - 1.0f) / (float)num_segments;
	drawList->PathArcTo(offset + centre * scale, radius * scale, a_max.x, a_max.y, num_segments);
	drawList->PathStroke(col, false, 1.f);
}

void ImDrawListScaled::AddSegmetFilledScaled(const ImVec2& centre, float radius, ImU32 col, ImVec2 radian, int num_segments)
{
	if ((col >> 24) == 0)
		return;

	const ImVec2 a_max = (radian);// *((float)num_segments - 1.0f) / (float)num_segments;
	drawList->PathArcTo(offset + centre * scale, radius * scale, a_max.x, a_max.y, num_segments);
	drawList->PathFill(col);
}