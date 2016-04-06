#pragma once


#include "../../3rdparty/imgui/imgui.h"
#include "util.h"
#include "field.h"
#include <cmath>
#include <cstdio>

#ifndef FAT_SPATIAL
#define FAT_SPATIAL 0.00001f
#endif // !FAT_SPATIAL

#ifndef FAT_ANGULAR
#define FAT_ANGULAR 0.0000001f
#endif // !FAT_ANGULAR



class Robot {
	ImDrawList* draw_list;
	int number;
	ImColor* pattern;
	ImVec2 rIdealPos;
	float rYaw;
	ImVec2 rIdealSeg;
	float fatigue;
	float battery;
	float radius;

public:
	void setNum(int num){}
	void put_degree(ImVec2 pos, float yaw, bool calculateFat = true) {
		ImVec2 vDelta = this->rIdealPos - pos;
		float fDelta = sqrt(pow(vDelta.x, 2.0f) + pow(vDelta.y, 2.0f));
		this->rIdealPos = pos;
		float old_yaw = this->rYaw;
		this->rYaw = yaw * 8 * atan(1.0f) /360.0f;
		old_yaw = abs(this->rYaw - old_yaw);
		this->fatigue += calculateFat ? fDelta * FAT_SPATIAL + old_yaw * FAT_ANGULAR : 0;
	}
	void put_radian(ImVec2 pos, float yaw, bool calculateFat = true) {
		ImVec2 vDelta = this->rIdealPos - pos;
		float fDelta = sqrt(pow(vDelta.x, 2.0f) + pow(vDelta.y, 2.0f));
		this->rIdealPos = pos;
		float old_yaw = this->rYaw;
		this->rYaw = yaw;
		old_yaw = abs(this->rYaw - old_yaw);
		this->fatigue += calculateFat ? fDelta * FAT_SPATIAL + old_yaw * FAT_ANGULAR : 0;
	}

	void setFatigue(float fat) {
		this->fatigue = fat;
	}

	float getFatigue() { return this->fatigue; }

	void setBattery(float bat) {
		this->battery = bat;
	}

	float getBattery() { return this->battery; }

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
		char* fatigue = new char[255];
		sprintf(rNum,"%x", this->number);
		sprintf(fatigue, "%f", this->fatigue);
		this->draw_list->PathArcTo(curPos, curRadius, this->rIdealSeg.x + this->rYaw, this->rIdealSeg.y + this->rYaw, 360);
		this->draw_list->PathFill(ImColor(0, 0, 0, 100));
		this->draw_list->AddText(
			ImGui::GetWindowFont(), 
			ImGui::GetWindowFontSize()*2.0*zoom, 
			curPos - ImVec2(curRadius / 3.0f, curRadius / 1.75f), 
			this->pattern[0], 
			rNum, 
			NULL
			);
		this->draw_list->AddRectFilled(
			ImVec2(curPos.x - curRadius, curPos.y + curRadius + 2.0f*zoom),
			ImVec2(curPos.x - curRadius + (this->battery > 1.0f ? 1.0f : (this->battery < 0.0f ? 0.0f : this->battery)) * 2 * curRadius, curPos.y + curRadius + 4.0f*zoom),
			ImColor(0, 0, 255, 200));
		this->draw_list->AddRectFilled(
			ImVec2(curPos.x - curRadius, curPos.y + curRadius + 4.0f*zoom), 
			ImVec2(curPos.x - curRadius + (this->fatigue > 1.0f ? 1.0f: (this->fatigue < 0.0f ? 0.0f : this->fatigue)) * 2 * curRadius, curPos.y + curRadius + 6.0f*zoom),
			ImColor(255, 0, 0, 200));
	}
	Robot(int number, Team team = Team::Blue) {
		this->rIdealSeg = ImVec2(0.64159265358979323846f,IM_2PI - 0.64159265358979323846f);
		this->radius = 20;
		this->rIdealPos = ImVec2(0,0);
		this->number = number;
		this->rYaw = 0;
		this->fatigue = 0.0f;
		this->battery = 1.0f;
		this->pattern = new ImColor[5]();
		for (int i = 0; i < 5; i++)
		{
			pattern[i] = ImColor(255, 255, 255, 200);
		}
		this->pattern[0] = team ? ImColor(255, 255, 0, 200) : ImColor(0, 0, 255, 200);
	}
	~Robot() {
		delete this->draw_list;
	}
};