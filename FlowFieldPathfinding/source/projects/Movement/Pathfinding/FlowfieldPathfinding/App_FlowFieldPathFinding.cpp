
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
	SAFE_DELETE(m_pFlowField)
	for (auto& agent : m_Agents)
	{
		SAFE_DELETE(agent)
	}
	m_Agents.clear();
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
	m_pFlowField->InitializeGrid(m_Columns, m_Rows, m_CellSize, false, true);
	m_pFlowField->InitializeBuffer();

	m_GraphRenderer.SetNumberPrintPrecision(0);

	SpawnRandomAgents();
}

void App_FlowFieldPathFinding::Update(float deltaTime)
{
	if (m_EditGraphEnabled)
	{
			m_GridEditor.UpdateGraph(m_pFlowField);
			/*if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
				EditFieldOnMouseClick(InputMouseButton::eLeft);*/

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

	for (const auto& agent : m_Agents)
	{
		agent->TrimToWorld({ 0,0 }, { static_cast<float>(m_WorldWidth), static_cast<float>(m_WorldHeight) }, false);
		agent->SetLinearVelocity((m_pFlowField->GetDirection(agent->GetPosition())) * agent->GetMaxLinearSpeed());		
	}
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

	ImGui::Text("Flowfield: ");
	ImGui::Spacing();
	ImGui::Spacing();

	////Get influence map data
	ImGui::Checkbox("Enable graph editing", &m_EditGraphEnabled);
	ImGui::Checkbox("Render node values", &m_RenderNodeNumbers);
	ImGui::Checkbox("Render node direction", &m_RenderNodeDirections);

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
	if (m_RenderNodeNumbers)
	{
		m_GraphRenderer.RenderGraph(m_pFlowField, true, true, false, false);
		
	}
	if (m_RenderNodeDirections)
	{
		m_GraphRenderer.RenderGraph(m_pFlowField, true, false, false, false);
		RenderDirections();
	}
	if (!m_RenderNodeNumbers && !m_RenderNodeDirections)
	{
		m_GraphRenderer.RenderGraph(m_pFlowField, true, false, false, false);
	}
	

}

void App_FlowFieldPathFinding::EditFieldOnMouseClick(Elite::InputMouseButton mouseBtn) const
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
void App_FlowFieldPathFinding::SpawnRandomAgents()
{
	//delete old agents
	for (auto& agent : m_Agents)
	{
		SAFE_DELETE(agent)
	}
	m_Agents.clear();

	

	Vector2 randomPos{};
	for (int idx{}; idx < m_NrOfAgents; ++idx)
	{
		m_Agents.push_back(new SteeringAgent());
		randomPos.x = static_cast<float>(rand() % m_WorldWidth);
		randomPos.y = static_cast<float>(rand() % m_WorldHeight);
		m_Agents[idx]->SetPosition(randomPos);	
		m_Agents[idx]->SetMaxLinearSpeed(10.f);
		m_Agents[idx]->SetAutoOrient(true);
		m_Agents[idx]->SetMass(0.f);
	}
}
void App_FlowFieldPathFinding::RenderDirections() const
{
	for (auto r = 0; r < m_Rows; ++r)
	{
		for (auto c = 0; c < m_Columns; ++c)
		{
			int idx = r * m_Columns + c;
			Vector2 cellPos{ m_pFlowField->GetNodeWorldPos(idx) };
			int cellSize = m_CellSize;

			//Node
			//Get direction
			Vector2 direction = m_pFlowField->GetDirection(idx);
			
			DEBUGRENDERER2D->DrawDirection(cellPos, direction, m_CellSize / 2.f, { 1.f,1.f,1.f });
			DEBUGRENDERER2D->DrawCircle(cellPos + (direction * (m_CellSize / 2.f)), cellSize / 8.f, { 1.f,1.f,1.f },0.9f);	
		}
	}
}