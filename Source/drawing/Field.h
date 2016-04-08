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
		bool x = min == winSz.x ? true : false;
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
			this->draw_list->AddTriangleFilled(
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x, ((this->fIdealSz.y - size.y)*Field::fAspectRatio / 2) - radius),
				offsetPos + ImVec2(radius, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				col
				);
			this->draw_list->AddRectFilled(
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(radius, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				col
				);
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
				);
			this->draw_list->AddTriangleFilled(
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(radius, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x, ((this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2) + radius),
				col
				);
		}
		else {
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(u_min, u_max)*IM_PI,
				360
				);
			this->draw_list->AddTriangleFilled(
				offsetPos + ImVec2(x, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x - radius, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x, ((this->fIdealSz.y - size.y)*Field::fAspectRatio / 2) - radius),
				col
				);
			this->draw_list->AddRectFilled(
				offsetPos + ImVec2(x - radius, (this->fIdealSz.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				col
				);
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
				);
			this->draw_list->AddTriangleFilled(
				offsetPos + ImVec2(x, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x, ((this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2) + radius),
				offsetPos + ImVec2(x-radius, (this->fIdealSz.y + size.y / 2)*Field::fAspectRatio / 2),
				col
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
		const float darkener = 1.0f;
		// Field
		this->draw_list->AddRectFilled(windowPos, windowPos + fEnd, this->fColor);
		// Field border
		this->draw_list->AddRectFilled(windowPos, windowPos + ImVec2(fEnd.x, this->lSz.y), this->lColor);
		this->draw_list->AddRectFilled(windowPos, windowPos+ImVec2(this->lSz.x, fEnd.y), this->lColor);
		this->draw_list->AddRectFilled(windowPos + ImVec2(fEnd.x-this->lSz.x, 0), windowPos + fEnd, this->lColor);
		this->draw_list->AddRectFilled(windowPos + ImVec2(0, fEnd.y - this->lSz.y), windowPos + fEnd, this->lColor);
		// Midline
		this->draw_list->AddCircleFilled(windowPos + (fEnd/2), this->midFieldCircleRadius * Field::fAspectRatio, this->lColor, 360);
		this->draw_list->AddCircleFilled(windowPos + (fEnd / 2), this->midFieldCircleRadius * Field::fAspectRatio - this->lSz.x, darken(this->fColor, 0.3f), 360);
		this->draw_list->AddRectFilled(windowPos + ImVec2((fEnd.x - this->lSz.x) / 2, 0), windowPos + ImVec2((fEnd.x + this->lSz.x) / 2, fEnd.y), this->lColor);
		// Goal zones
		this->drawGZ(true, this->gzSz, this->lColor, windowPos);
		this->drawGZ(false, this->gzSz, this->lColor, windowPos);
		this->drawGZ(true, this->gzSz-this->lSz, this->fColor, windowPos);
		this->drawGZ(false, this->gzSz - this->lSz, this->fColor, windowPos);
		
		return;
	}

	Field(const ImVec2& fIdealSz = ImVec2(680.0f, 460.0f), const ImVec2& wMargin = ImVec2(40.0f, 40.0f)) {
		this->opened = true;
		this->fColor = IMC_GREEN;
		this->lColor = IMC_WHITE;
		this->lSz = ImVec2(3.0f,3.0f);
		this->gzSz = ImVec2(70, 100);
		this->midFieldCircleRadius = 40;
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
