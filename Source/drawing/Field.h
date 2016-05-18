#pragma once

#include "../../3rdparty/imgui/imgui.h"
#include "../utility/vector_helper.h"
#include "colors.h"
#include "util.h"
#include "IDrawable.h"



class Field : public IDrawable {

	ImDrawList* draw_list;
	float midFieldCircleRadius;
	ImVec2 windowIdealSize;
	ImVec2 fieldIdealSize;
	ImColor fieldColor;
	ImColor lineColor;
	ImVec2 gzSz;

	void drawGZ(bool side, ImVec2 size, ImColor col = ImColor(255, 255, 255, 200), ImVec2 offsetPos = ImVec2(0, 0)) {
		float u_min = side ? 1.500f : 1.000f;
		float u_max = side ? 2.000f : 1.500f;
		float b_min = side ? 0 : 0.500f;
		float b_max = side ? 0.500f : 1.000f;
		float radius = size.x*fAspectRatio;
		float x = side ? 0 : this->fieldIdealSize.x * fAspectRatio;
		if (side)
		{
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fieldIdealSize.y - size.y)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(u_min, u_max)*IM_PI,
				360);
			this->draw_list->AddLine(
				offsetPos + ImVec2(radius, (this->fieldIdealSize.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(radius, (this->fieldIdealSize.y + size.y / 2)*Field::fAspectRatio / 2),
				col,
				1.f);
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fieldIdealSize.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
			);
		}
		else {
			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fieldIdealSize.y - size.y)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(u_min, u_max)*IM_PI,
				10
			);

			this->draw_list->AddLine(
				offsetPos + ImVec2(x - radius, (this->fieldIdealSize.y - size.y)*Field::fAspectRatio / 2),
				offsetPos + ImVec2(x - radius, (this->fieldIdealSize.y + size.y / 2)*Field::fAspectRatio / 2),
				col,
				1.f);

			AddSegmetFilled(
				this->draw_list,
				offsetPos + ImVec2(x, (this->fieldIdealSize.y + size.y / 2)*Field::fAspectRatio / 2),
				radius,
				col,
				ImVec2(b_min, b_max)*IM_PI,
				360
			);
		}
	}


public:
	float fAspectRatio;
	void draw(ImDrawList* draw_list, float zoom, ImVec2 margin) override
	{
		this->draw_list = draw_list;
		Field::fAspectRatio = zoom;
		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 windowPos = (p + margin);
		const ImVec2 fEnd = this->fieldIdealSize*Field::fAspectRatio;

		// Field
		this->draw_list->AddRectFilled(windowPos - margin, windowPos + margin + fEnd, this->fieldColor);
		// Field border
		this->draw_list->AddRect(windowPos, windowPos + ImVec2(fEnd.x, fEnd.y), this->lineColor);
		// Midline
		this->draw_list->AddLine(windowPos + ImVec2(fEnd.x / 2.f, 0.f), windowPos + ImVec2(fEnd.x / 2.f, fEnd.y), this->lineColor);
		this->draw_list->AddCircle(windowPos + (fEnd / 2), this->midFieldCircleRadius * Field::fAspectRatio, this->lineColor, 360);
		// Goal zones
		this->drawGZ(true, this->gzSz, this->lineColor, windowPos);
		this->drawGZ(false, this->gzSz, this->lineColor, windowPos);

		return;
	}

	Field(const ImVec2& fIdealSz = ImVec2(900.0f, 600.0f), const ImVec2& wMargin = ImVec2(60.0f, 60.0f)) {
		this->fieldColor = IMC_FIELD_GREEN;
		this->lineColor = IMC_WHITE;
		this->gzSz = ImVec2(70, 100);
		this->midFieldCircleRadius = 50;
		this->fieldIdealSize = fIdealSz;
		this->windowIdealSize = this->fieldIdealSize + wMargin;
	}
};

