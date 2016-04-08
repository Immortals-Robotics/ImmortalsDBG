#pragma once
#include "../../3rdparty/imgui/imgui.h"
#include "vectors.h"

#ifndef IM_COLORS
#define	IM_COLORS


#define	IMC_WHITE	ImColor(255,255,255,255)
#define	IMC_BLACK	ImColor(0,0,0,255)
#define	IMC_RED		ImColor(255,0,0,255)
#define IMC_GREEN	ImColor(0,255,0,255)
#define IMC_BLUE	ImColor(0,0,255,255)
#define IMC_YELLOW	ImColor(255,255,0,255)
#define IMC_ORANGE	ImColor(255,170,20,255)
#endif // !IM_COLORS


const ImColor darken(const ImColor& a, float level) {
	level = abs(level) > 1.0f ? 1.0f : abs(level);
	ImColor fin = a.Value * level;
	return ImColor(fin.Value.z, fin.Value.z, fin.Value.z, a.Value.w);
}

const ImColor lighten(const ImColor& a, float level) {
	level = abs(level) > 1.0f ? 1.0f : abs(level);
	if (level == 0.0f) return a;
	ImColor fin = a.Value / level;
	return ImColor(
		fin.Value.x > 1.0f ? 1.0f : fin.Value.x, 
		fin.Value.y > 1.0f ? 1.0f : fin.Value.y, 
		fin.Value.z > 1.0f ? 1.0f : fin.Value.z, 
		a.Value.w);
}