#ifndef INFLUENCE_MAP_APPLICATION_H
#define INFLUENCE_MAP_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"

//--- Includes and Forward Declarations ---
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework\EliteAI\EliteGraphs\EInfluenceMap.h"
#include "framework\EliteAI\EliteGraphs\EGraph2D.h"
#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphEditor.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"
#include "projects/Movement/Pathfinding/FlowfieldPathfinding/FlowField/FlowField.h"

class NavigationColliderElement;
class SteeringAgent;
class Seek;
class Flee;
class Wander;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_FlowFieldPathFinding final : public IApp
{
public:
	//Constructor & Destructor
	App_FlowFieldPathFinding() = default;
	virtual ~App_FlowFieldPathFinding() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void UpdateUI();
	void Render(float deltaTime) const override;


private:
	Elite::FlowField* m_pFlowField = nullptr;

	Elite::GraphEditor m_WaypointGraphEditor{};
	Elite::GraphEditor m_GridEditor{};
	Elite::GraphRenderer m_GraphRenderer{};

	bool m_EditGraphEnabled = false;
	bool m_RenderAsGraph = false;

	void EditFieldOnMouseClick(Elite::InputMouseButton mouseBtn);
private:
	//C++ make the class non-copyable
	App_FlowFieldPathFinding(const App_FlowFieldPathFinding&) = delete;
	App_FlowFieldPathFinding& operator=(const App_FlowFieldPathFinding&) = delete;
};
#endif