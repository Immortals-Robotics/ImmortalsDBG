#pragma once

#include "../../3rdparty/imgui/imgui.h"
#include "vectors.h"
#include "colors.h"
#include "util.h"
#include "IDrawable.h"

class Ball : public IDrawable {
	ImDrawList* draw_list; 
	ImVec2 idealPos;
	float radius;
	ImColor color;


public:

	void put(ImVec2 pos) {
		this->idealPos = pos;
	}

	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin) {
		this->draw_list = draw_list;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		ImVec2 curPos = windowPos + this->idealPos * zoom;
		float curRadius = this->radius * zoom;
		this->draw_list->AddCircleFilled(curPos, curRadius, this->color, 10);
		return;
	}

	Ball() {
		this->radius = 2.15f;
		this->idealPos = ImVec2(100,100);
		this->color = IMC_ORANGE;
	}
	~Ball() {}
};