#pragma once

#include "../../3rdparty/imgui/imgui.h"
#include "vectors.h"
#include "colors.h"
#include "util.h"
#include "Drawable.h"

#ifndef FAT_SPATIAL
#define FAT_SPATIAL 0.00001f
#endif // !FAT_SPATIAL

#ifndef FAT_ANGULAR
#define FAT_ANGULAR 0.0000001f
#endif // !FAT_ANGULAR



class Robot : public Drawable {
	ImDrawList* draw_list;
	int number;
	ImColor pattern;
	ImColor base;
	ImColor* stat;
	ImVec2 idealPos;
	float yaw;
	ImVec2 idealSeg;
	float fatigue;
	float battery;
	float radius;

	enum Stat {
		Battery = 0,
		Fatigue = 1
	};

public:
	void setNum(int num){}
	void put_degree(ImVec2 pos, float yaw, bool calculateFat = true) {
		ImVec2 vDelta = this->idealPos - pos;
		float fDelta = sqrt(pow(vDelta.x, 2.0f) + pow(vDelta.y, 2.0f));
		this->idealPos = pos;
		float old_yaw = this->yaw;
		this->yaw = yaw * 8 * atan(1.0f) /360.0f;
		old_yaw = abs(this->yaw - old_yaw);
		this->fatigue += calculateFat ? fDelta * FAT_SPATIAL + old_yaw * FAT_ANGULAR : 0;
	}
	void put_radian(ImVec2 pos, float yaw, bool calculateFat = true) {
		ImVec2 vDelta = this->idealPos - pos;
		float fDelta = sqrt(pow(vDelta.x, 2.0f) + pow(vDelta.y, 2.0f));
		this->idealPos = pos;
		float old_yaw = this->yaw;
		this->yaw = yaw;
		old_yaw = abs(this->yaw - old_yaw);
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
	ImVec2 getPos() { return this->idealPos; }
	float getYaw_radian() { return this->yaw; }
	float getYaw_degree() { return this->yaw*360/IM_2PI; }


	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin) {
		this->draw_list = draw_list;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		ImVec2 curPos = windowPos + this->idealPos * zoom;
		float curRadius = this->radius * zoom;
		char* rNum = new char();
		//char* fatigue = new char[9];
		//int x = strlen(fatigue);
		sprintf(rNum ,"%x", this->number);
		//sprintf_s(fatigue, strlen(fatigue),"%f", this->fatigue);
		//fatigue[8] = '\0';
		this->draw_list->PathArcTo(curPos, curRadius, this->idealSeg.x + this->yaw, this->idealSeg.y + this->yaw, 18);
		this->draw_list->PathFill(this->base);
		this->draw_list->AddText(
			ImGui::GetWindowFont(), 
			ImGui::GetWindowFontSize()*zoom,
			curPos - ImVec2(curRadius / 3.0f, curRadius / 1.75f),
			this->pattern, 
			rNum,
			NULL);

		this->draw_list->AddRectFilled(
			ImVec2(curPos.x - curRadius, curPos.y + curRadius + 2.0f*zoom),
			ImVec2(curPos.x - curRadius + (this->battery > 1.0f ? 1.0f : (this->battery < 0.0f ? 0.0f : this->battery)) * 2 * curRadius, curPos.y + curRadius + 6.0f*zoom),
			this->stat[Stat::Battery]);

		this->draw_list->AddRect(
				ImVec2(curPos.x - curRadius, curPos.y + curRadius + 2.0f*zoom),
				ImVec2(curPos.x + curRadius, curPos.y + curRadius + 6.0f*zoom),
				IMC_BLACK);
		/*this->draw_list->AddRectFilled(
			ImVec2(curPos.x - curRadius, curPos.y + curRadius + 4.0f*zoom), 
			ImVec2(curPos.x - curRadius + (this->fatigue > 1.0f ? 1.0f: (this->fatigue < 0.0f ? 0.0f : this->fatigue)) * 2 * curRadius, curPos.y + curRadius + 6.0f*zoom),
			this->stat[Stat::Fatigue]);*/
	}

	Robot(int number, Team team = Team::Blue) {
		this->idealSeg = ImVec2(0.64159265358979323846f,IM_2PI - 0.64159265358979323846f);
		this->radius = 9;
		this->idealPos = ImVec2(0,0);
		this->number = number;
		this->yaw = 0;
		this->fatigue = 0.0f;
		this->battery = 0.7f;
		this->pattern = team ? IMC_BLACK : IMC_WHITE;
		this->base = team ? IMC_YELLOW : IMC_BLUE;
		this->stat = new ImColor[2];
		this->stat[Stat::Battery] = IMC_CYAN;
		this->stat[Stat::Fatigue] = IMC_RED;
	}
	~Robot() {
		delete this->draw_list;
	}
};