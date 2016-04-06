#pragma once


#include "../../3rdparty/imgui/imgui.h"
#include "util.h"
#include "field.h"
#include <cmath>
#include <cstdio>

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
	void put_degree(ImVec2 pos, float yaw) {
		this->rIdealPos = pos;
		this->rYaw = yaw * 8 * atan(1.0f) /360.0f;
	}
	void put_radian(ImVec2 pos, float yaw) {
		this->rIdealPos = pos;
		this->rYaw = yaw;
	}

	ImVec2 getPos() { return this->rIdealPos; }
	float getYaw_radian() { return this->rYaw; }
	float getYaw_degree() { return this->rYaw*360/IM_2PI; }
	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin = ImVec2(20.0f, 20.0f)) {
		this->draw_list = draw_list;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		ImVec2 curPos = windowPos + this->rIdealPos * zoom;
		float curRadius = this->radius * zoom;
		char * rNum = new char();
		sprintf(rNum,"%x", this->number);
		this->draw_list->PathArcTo(curPos, curRadius, this->rIdealSeg.x + this->rYaw, this->rIdealSeg.y + this->rYaw, 360);
		this->draw_list->PathFill(ImColor(0, 0, 0, 100));
		this->draw_list->AddText(ImGui::GetWindowFont(), ImGui::GetWindowFontSize()*2.0*zoom, curPos - ImVec2(curRadius / 3.0f, curRadius / 1.75f), ImColor(255, 255, 255, 255), rNum, NULL);
	}
	Robot(int number) {
		this->rIdealSeg = ImVec2(0.64159265358979323846f,IM_2PI - 0.64159265358979323846f);
		this->radius = 20;
		this->rIdealPos = ImVec2(0,0);
		this->number = number;
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