
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
using namespace Elite;

//Includes
#include "App_FlowFieldPathFinding.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

//Destructor
App_FlowFieldPathFinding::~App_FlowFieldPathFinding()
{
	SAFE_DELETE(m_pFlowField);
}

//Functions
void App_FlowFieldPathFinding::Start()
{
	//Initialization of your application. If you want access to the physics world you will need to store it yourself.
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(50.f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(70, 50));
	DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(true);

	m_pFlowField = new FlowField(false);
	m_pFlowField->InitializeGrid(16, 16, 8, false, true);
	m_pFlowField->InitializeBuffer();

	m_GraphRenderer.SetNumberPrintPrecision(0);
}

void App_FlowFieldPathFinding::Update(float deltaTime)
{
	if (m_EditGraphEnabled)
	{
			m_GridEditor.UpdateGraph(m_pFlowField);
			if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
				EditFieldOnMouseClick(InputMouseButton::eLeft);

			if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
				EditFieldOnMouseClick(InputMouseButton::eRight);
	}
	else
	{
		//------ INPUT ------
		if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
			EditFieldOnMouseClick(InputMouseButton::eLeft);
	}

	UpdateUI();
}

void App_FlowFieldPathFinding::UpdateUI()
{
	//Setup
	int menuWidth = 200;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);
	ImGui::SetWindowFocus();
	ImGui::PushItemWidth(70);
	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Influence Maps");
	ImGui::Spacing();
	ImGui::Spacing();

	////Get influence map data
	ImGui::Checkbox("Enable graph editing", &m_EditGraphEnabled);
	ImGui::Checkbox("Render as graph", &m_RenderAsGraph);

	//auto momentum = m_pInfluenceGrid->GetMomentum();
	//auto decay = m_pInfluenceGrid->GetDecay();
	//auto propagationInterval = m_pInfluenceGrid->GetPropagationInterval();

	//ImGui::SliderFloat("Momentum", &momentum, 0.0f, 1.f, "%.2");
	//ImGui::SliderFloat("Decay", &decay, 0.f, 1.f, "%.2");
	//ImGui::SliderFloat("Propagation Interval", &propagationInterval, 0.f, 2.f, "%.2");
	//ImGui::Spacing();

	//Set data
	//m_pInfluenceGrid->SetMomentum(momentum);
	//m_pInfluenceGrid->SetDecay(decay);
	//m_pInfluenceGrid->SetPropagationInterval(propagationInterval);
	//
	//m_pInfluenceGraph2D->SetMomentum(momentum);
	//m_pInfluenceGraph2D->SetDecay(decay);
	//m_pInfluenceGraph2D->SetPropagationInterval(propagationInterval);

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
}

void App_FlowFieldPathFinding::Render(float deltaTime) const
{
	m_GraphRenderer.RenderGraph(m_pFlowField, true, true, false, false);

}

void App_FlowFieldPathFinding::EditFieldOnMouseClick(Elite::InputMouseButton mouseBtn)
{
	auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, mouseBtn);
	auto mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(Vector2{ (float)mouseData.X, (float)mouseData.Y });

	if (mouseBtn == InputMouseButton::eRight)
	{
		m_pFlowField->ToggleWall(mousePos);
	}
	else
	{
		m_pFlowField->SetPathPoint(mousePos);
	}
	
	//	m_pInfluenceGraph2D->SetInfluenceAtPosition(mousePos, inf);

}
