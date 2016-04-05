#pragma once


#include "../../3rdparty/imgui/imgui.h"
#include "util.h"
#include "field.h"
#include <cmath>

class Robot {
	ImDrawList* draw_list;
	int number;
	ImColor* pattern;
	ImVec2 rIdealPos;
	float rYaw;
	ImVec2 rIdealSeg;
	float radius;
	Field f;

public:
	void setNum(int num){}
	void put(ImVec2 pos, float yaw) {
		this->rIdealPos = pos;
		this->rYaw = yaw* 8 * atan(1.0f) /360.0f;
	}
	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin = ImVec2(20.0f, 20.0f)) {
		this->draw_list = draw_list;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		this->put(ImVec2(100, 100), 0);
		this->draw_list->PathArcTo(windowPos + this->rIdealPos * Field::fAspectRatio, this->radius, 20.0f + this->rYaw, 340.0f + this->rYaw, 360);
		this->draw_list->PathFill(ImColor(0, 0, 0, 200));
		/*
		AddSegmetFilled(
			this->draw_list,
			windowPos + this->rIdealPos * Field::fAspectRatio,
			this->radius,
			ImColor(0, 0, 0,200),
			ImVec2(0.25f + this->rYaw, 1.75f + this->rYaw)*4*atan(1.0f),
			360
			);
			*/
	}
	Robot() {
		this->rIdealSeg = ImVec2();
		this->radius = 20;
		this->rIdealPos = ImVec2(0,0);
		this->rYaw = 0;
		this->pattern = new ImColor[5]();
		for (int i = 0; i < 5; i++)
		{
			pattern[i] = ImColor(255, 255, 255, 200);
		}
	}
	~Robot() {
		delete this->draw_list;
	}
};