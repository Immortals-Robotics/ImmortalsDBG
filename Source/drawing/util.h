#pragma once

#include "../../3rdparty/imgui/imgui.h"

#ifndef IM_PI
#define IM_PI					3.14159265358979323846f
#endif // !
#ifndef IM_2PI
#define IM_2PI					6.28318530717958647692f
#endif // !


const ImVec2 operator +(const ImVec2& a, const ImVec2& b) {
	return ImVec2(a.x + b.x, a.y + b.y);
}

const ImVec2 operator +=(ImVec2& a, const ImVec2& b) {
	return a = ImVec2(a.x + b.x, a.y + b.y);
}

const ImVec2 operator -(const ImVec2& a, const ImVec2& b) {
	return ImVec2(a.x - b.x, a.y - b.y);
}

const ImVec2 operator -=(ImVec2& a, const ImVec2& b) {
	return a = ImVec2(a.x - b.x, a.y - b.y);
}

const ImVec2 operator *(const ImVec2& a, const ImVec2& b) {
	return ImVec2(a.x * b.x, a.y * b.y);
}

const ImVec2 operator *=(ImVec2& a, const ImVec2& b) {
	return a = ImVec2(a.x * b.x, a.y * b.y);
}

const ImVec2 operator *(const ImVec2& a, const float& b) {
	return ImVec2(a.x * b, a.y * b);
}

const ImVec2 operator *=(ImVec2& a, const float& b) {
	return a = ImVec2(a.x * b, a.y * b);
}

const ImVec2 operator /(const ImVec2& a, const float& b) {
	return ImVec2(a.x / b, a.y / b);
}

const ImVec2 operator /=(ImVec2& a, const float& b) {
	return a = ImVec2(a.x / b, a.y / b);
}

const ImColor darken(const ImColor& a, float level) {
	ImVec4 val = a.Value;
	return ImColor((a.Value.x - level) > 0 ? (a.Value.x - level) : 0,
		(a.Value.y - level) > 0 ? (a.Value.y - level) : 0,
		(a.Value.z - level) > 0 ? (a.Value.z - level) : 0,
		val.w);
}

void AddSegmetFilled(ImDrawList* x, const ImVec2& centre, float radius, ImU32 col, ImVec2 radian, int num_segments)
{
	if ((col >> 24) == 0)
		return;

	const ImVec2 a_max = (radian);// *((float)num_segments - 1.0f) / (float)num_segments;
	x->PathArcTo(centre, radius, a_max.x, a_max.y, num_segments);
	x->PathFill(col);
}