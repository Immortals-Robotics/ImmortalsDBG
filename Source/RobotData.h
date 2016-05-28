#include <vector>
#include <queue>
#include "../3rdparty/imgui/imgui.h"
#include "robot-feedback.pb.h"

#define max_history_size 100
#define full_battery_voltage 16.8f
#define empty_battery_voltage 7.8f

using namespace std;
std::queue <vector<RobotFeedback>>RobotDataHistory;

inline void QueueRobotData(const vector<RobotFeedback>& data)
{
	RobotDataHistory.push(data);
	if (RobotDataHistory.size() > max_history_size)
		RobotDataHistory.pop();
}

inline void DrawRobotDatas(const vector<RobotFeedback>& data)
{
	ImGui::Columns(5, "mixed");
	ImGui::Separator();
	const char* headers[7] = { "#R","Battery","Enc 1\\2\\3\\4","M 1\\2\\3\\4","dribller","Cheap","Err" };
	const float columnsWidth[7] = { 2,6,3,3,1,1,1 };
	char* TempString = new char[100];
	char TempStringA[] = "0\\0\\0\\0";
	ImColor buttonColor;
	const float totalColumnsSize = ImGui::GetContentRegionAvailWidth();
	float columnOffset = 0.0f;
	static bool firstTime = true;
	for (int i = 0; i < 5; i++)
	{
		ImGui::Text(headers[i]);
		ImGui::NextColumn();
	}
	firstTime = false;
	ImGui::Separator();
	for (auto robot : data)
	{
		//////Robot ID
		sprintf(TempString, "%d", robot.robot_id());
		ImGui::Text(TempString);
		ImGui::NextColumn();

		//////Battery
		//ImGui::Text(robotData[j][i]); ImGui::SameLine(30);
		float batteryInt = 1.0f - ((full_battery_voltage - robot.battery_voltage()) / (full_battery_voltage - empty_battery_voltage));	//normalize battery voltage 
		buttonColor = ImColor(1.0f - batteryInt, batteryInt, 0.2f);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
		//ImGui::PushItemWidth(200.0f*batteryInt);
		//printf("%f\n", ImGui::GetContentRegionAvailWidth()*batteryInt);
		//ImGui::TextColored(ImVec4(batteryInt, batteryInt, batteryInt, 1.0f), robotData[j][i]);
		ImGui::Button("", ImVec2(ImGui::GetContentRegionAvailWidth()*batteryInt, 0));
		//ImGui::PopItemWidth(); 
		ImGui::PopStyleColor(3);
		ImGui::NextColumn();

		//////Encoder Fault
		sprintf(TempStringA, "0\\0\\0\\0");
		for (int i = 0; i < 4; i++) //&& i<robot.encoder_fault_size
		{
			if (robot.encoder_fault(i))
				TempStringA[2 * i] = '1';
		}
		ImGui::Text(TempStringA);
		ImGui::NextColumn();

		//////Motor Fault
		sprintf(TempStringA, "0\\0\\0\\0");
		bool alert = false;
		for (int i = 0; i < 4; i++) //&& i<robot.motor_fault_size
		{
			if (robot.motor_fault(i))
			{
				TempStringA[2 * i] = '1';
				alert = true;
			}
		}
		if (alert)
		{
			buttonColor = ImColor(0.8f, 0.0f, 0.2f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			buttonColor = ImColor(0.0f, 0.8f, 0.2f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
		ImGui::Button(TempStringA);
		ImGui::PopStyleColor(4);
		ImGui::NextColumn();

		//////dribber connection status
		if (robot.dribbe_connected())
		{
			buttonColor = ImColor(0.0f, 0.8f, 0.2f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			buttonColor = ImColor(0.8f, 0.0f, 0.2f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
		ImGui::Button(robot.dribbe_connected() ? "Yes" : "No");
		ImGui::PopStyleColor(4);
		ImGui::NextColumn();

		//////!!!!!!!!!!!!!!!		kick and cheap should be determined from enviroment			!!!!!!!!!!!!!!!!!!!!!!!!

		ImGui::Separator();
	}


	ImGui::TreePop();
}