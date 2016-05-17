#pragma once

#include "../../3rdparty/imgui/imgui.h"
#include "vectors.h"
#include "colors.h"
#include "util.h"
#include "IDrawable.h"



class Field : public IDrawable {

	ImDrawList* draw_list;
	float wAspectRatio;
	float midFieldCircleRadius;
	ImVec2 wIdealSz;
	ImVec2 fIdealSz;
	ImColor fColor;
	ImVec2 lSz;
	ImColor lColor;
	ImVec2 gzSz;

	void resize() {
		ImVec2 winSz = ImGui::GetWindowSize();
		float min = winSz.x > winSz.y ? winSz.y : winSz.x;
		bool x = (winSz.x < winSz.y);
		this->wAspectRatio = x ? winSz.x / this->wIdealSz.x : winSz.y / this->wIdealSz.y;
		Field::fAspectRatio = this->wAspectRatio;
	}

	void drawGZ(bool side, ImVec2 size, ImColor col = ImColor(255,255,255,200), ImVec2 offsetPos = ImVec2(0,0)) {
		float u_min = side ? 1.500f : 1.000f;
		float u_max = side ? 2.000f : 1.500f;
		float b_min = side ? 0 : 0.500f;
		float b_max = side ? 0.500f : 1.000f;
		float radius = size.x*Field::fAspectRatio;
		float x = side ? 0 : this->fIdealSz.x * Field::fAspectRatio;
		if (side)
		{
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(u_min, u_max)*IM_PI,
				360
				);
			this->draw_list->AddLine(
				offsetPos + ImVec2(radius, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(radius, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				col,
				1.f);
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
				);
		}
		else {
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(u_min, u_max)*IM_PI,
				10
				);

			this->draw_list->AddLine(
				offsetPos + ImVec2(x - radius, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x - radius, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				col,
				1.f);

			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
				);
		}
	}
	

public:
	static float fAspectRatio;
	bool opened;
	ImVec2 wSize;
	ImVec2 fSize;
	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin) {
		this->draw_list = draw_list;
		Field::fAspectRatio = zoom;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		const ImVec2 fEnd = this->fIdealSz*Field::fAspectRatio;

		// Field
		this->draw_list->AddRectFilled(windowPos - margin, windowPos + margin + fEnd, this->fColor);
		// Field border
		this->draw_list->AddRect(windowPos, windowPos + ImVec2(fEnd.x, fEnd.y), this->lColor);
		// Midline
		this->draw_list->AddLine(windowPos + ImVec2(fEnd.x / 2.f, 0.f), windowPos + ImVec2(fEnd.x / 2.f, fEnd.y), this->lColor);
		this->draw_list->AddCircle(windowPos + (fEnd/2), this->midFieldCircleRadius * Field::fAspectRatio, this->lColor, 360);
		// Goal zones
		this->drawGZ(true, this->gzSz, this->lColor, windowPos);
		this->drawGZ(false, this->gzSz, this->lColor, windowPos);
		
		return;
	}

	Field(const ImVec2& fIdealSz = ImVec2(900.0f, 600.0f), const ImVec2& wMargin = ImVec2(60.0f, 60.0f)) {
		this->opened = true;
		this->fColor = ImColor(0.0f, 0.55f, 0.0f);
		this->lColor = IMC_WHITE;
		this->lSz = ImVec2(3.0f,3.0f);
		this->gzSz = ImVec2(70, 100);
		this->midFieldCircleRadius = 50;
		this->fIdealSz = fIdealSz;
		this->wIdealSz = this->fIdealSz + wMargin;
		this->fSize = this->fIdealSz;
		this->wSize = this->wIdealSz;
		Field::fAspectRatio = 1.0f;
		this->wAspectRatio = 1.0f;
		return;
	}
	~Field() {
		delete this->draw_list;
		return;
	}
};

float Field::fAspectRatio = 1.0f;
