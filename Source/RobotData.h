#include <vector>
#include "../3rdparty/imgui/imgui.h"

using namespace std;

struct RobotData
{
	uint16_t RobotID;
	float BatteryVoltage;
	bool ChipOK, KickOK, Error, IsInFiled;
	bool MotorFault[4], EncoderFault[4];
};

inline void DrawRobotDatas(const vector<RobotData>& data)
{
	for (auto robot : data)
	{
		char *s;
		sprintf(s, "%d", robot.RobotID);
		ImGui::Text(s);
		ImGui::NextColumn();

		float batteryInt = 1.0f - ((16.8f - robot.BatteryVoltage) / 7.0f);	//full=16.8 emp=9.8
		ImColor buttonColor = ImColor(1.0f - batteryInt, batteryInt, 0.2f);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
		//ImGui::PushItemWidth(200.0f*batteryInt);
		//printf("%f\n", ImGui::GetContentRegionAvailWidth()*batteryInt);
		//ImGui::TextColored(ImVec4(batteryInt, batteryInt, batteryInt, 1.0f), robotData[j][i]);
		ImGui::Button("", ImVec2(ImGui::GetContentRegionAvailWidth()*batteryInt, 0));
		//ImGui::PopItemWidth(); 
		ImGui::PopStyleColor(3);
	}
}