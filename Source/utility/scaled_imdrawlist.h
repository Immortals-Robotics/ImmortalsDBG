#include "../../3rdparty/imgui/imgui.h"

struct ImDrawListScaled
{
	ImDrawListScaled();

	ImDrawList* drawList;

	ImVec2 offset;
	float scale;

	void  AddLineScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
	void  AddRectScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners = 0x0F);        // a: upper-left, b: lower-right
	void  AddRectFilledScaled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners = 0x0F);  // a: upper-left, b: lower-right
	void  AddCircleScaled(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
	void  AddCircleFilledScaled(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
	void  AddTextScaled(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
	void AddSegmetScaled(const ImVec2 &centre, float radius, ImU32 col, ImVec2 radian, int num_segments);

	void AddSegmetFilledScaled(const ImVec2 &centre, float radius, ImU32 col, ImVec2 radian, int num_segments);
};