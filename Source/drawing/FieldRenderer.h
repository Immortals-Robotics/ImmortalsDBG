//
// Created by lordhippo on 5/18/16.
//

#ifndef IMMDBG_DRAWBACKEND_H
#define IMMDBG_DRAWBACKEND_H


#include "../../3rdparty/imgui/imgui.h"
#include "../protos/messages_robocup_ssl_geometry_legacy.pb.h"
#include "../protos/messages_robocup_ssl_detection.pb.h"
#include "../utility/scaled_imdrawlist.h"

enum TeamColor {
	Blue = 0,
	Yellow = 1,
};

class FieldRenderer {
private:
	ImDrawListScaled* drawList;
	ImVec2 widgetPos;
	ImVec2 widgetSize;
	ImVec2 overallFieldSize;

	const float ballRadius;
	const float robotRadius;
	const float robotArcAngle;

	void CalculateZoom();

public:
	FieldRenderer();
	void SetDrawList(ImDrawList* const drawList);
	void SetWidgetProperties(const ImVec2 &pos, const ImVec2 &size);
	void SetFieldSizeLegacy(const RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize& field);

	void DrawFieldLegacy(const RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize& data) const ;
	void DrawCameraCalibrations(const google::protobuf::RepeatedPtrField<SSL_GeometryCameraCalibration>& data) const ;

	void DrawBalls(const google::protobuf::RepeatedPtrField<SSL_DetectionBall>& data) const ;
	void DrawRobots(const google::protobuf::RepeatedPtrField<SSL_DetectionRobot>& data, TeamColor color) const ;
};


#endif //IMMDBG_DRAWBACKEND_H
