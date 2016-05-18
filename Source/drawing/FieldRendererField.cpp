//
// Created by lordhippo on 5/18/16.
//

#include "FieldRenderer.h"

#include "../utility/vector_helper.h"

void FieldRenderer::DrawField(const SSL_GeometryFieldSize& data) const
{
	// Field
	this->drawList->AddRectFilled(
			ImVec2(0.f, 0.f),
			this->overallFieldSize,
			this->fieldColor);

	auto lFieldStartPos = ImVec2(data.boundary_width() + data.referee_width(),
								 data.boundary_width() + data.referee_width());
	auto lFieldEndPos = lFieldStartPos + ImVec2(data.field_length(), data.field_width());
	auto lFieldCenter = (lFieldStartPos+lFieldEndPos) / 2.f;

	// Field border
	this->drawList->AddRect(
			this->widgetPos + lFieldStartPos,
			this->widgetPos + lFieldEndPos,
			this->lineColor);
	// Midline
	this->drawList->AddLine(
			this->widgetPos + ImVec2(lFieldCenter.x, lFieldStartPos.y),
			this->widgetPos + ImVec2(lFieldCenter.x, lFieldEndPos.y),
			this->lineColor);
	this->drawList->AddCircle(
			this->widgetPos + lFieldCenter,
			data.center_circle_radius(),
			this->lineColor,
			18);

	// TODO: Penalty area
}
void FieldRenderer::DrawCameraCalibrations(const google::protobuf::RepeatedPtrField<SSL_GeometryCameraCalibration>& data) const
{

}