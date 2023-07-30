#pragma once

#include "framework\EliteAI\EliteGraphs\EGridGraph.h"
#include "projects/Shared/NavigationColliderElement.h"
#include <algorithm>
#include <memory>


namespace Elite
{
	class FlowField final : public GridGraph<InfluenceNode, GraphConnection>
	{
	public:
		FlowField(bool isDirectional) : GridGraph(isDirectional) {}
		void InitializeBuffer() {
			m_DijkstraGrid = std::vector<int>(m_Nodes.size());	
			m_DirectionBuffer = std::vector<Elite::Vector2>(m_Nodes.size());
			
		}
		void Render() const {}

		void SetPathPoint(Elite::Vector2 pos);
		void ToggleWall(Elite::Vector2 pos);


	protected:
		virtual void OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged) override;

	private:
		void CalculateDijkstra(int idx);
		void CalculateDirectionVectors();
		std::vector<int> GetNeighbors(int idx);
		std::vector<int> m_DijkstraGrid;
		std::vector<Elite::Vector2> m_DirectionBuffer;
		std::vector<std::unique_ptr <NavigationColliderElement>> m_pWalls{};
		std::vector<int> m_pWallIdx{};
	};

	inline void FlowField::CalculateDijkstra(int idx)
	{
		//Generate empty grid
		std::fill(m_DijkstraGrid.begin(), m_DijkstraGrid.end(), NULL);

		//Set places with walls as Max
		for (const auto& Wall : m_pWallIdx)
		{
			m_DijkstraGrid[Wall] = -1;
		}

		//FloodFill from endpoint
		std::deque<int> toVisit{};
		toVisit.push_back(idx);
		m_DijkstraGrid[idx] = 1;

		while (!toVisit.empty())
		{
			const int currentIdx = toVisit.front();

			//get neighbors
			std::vector<int> neighbors = GetNeighbors(currentIdx);
			toVisit.pop_front();

			//for each neighbor of node
			for (const auto& neighbor : neighbors)
			{
				if (m_DijkstraGrid[neighbor] == NULL)
				{
					m_DijkstraGrid[neighbor] = m_DijkstraGrid[currentIdx] + 1;
					toVisit.push_back(neighbor);
				}
			}
		}
		for (auto& node : m_Nodes)
		{
			node->SetInfluence(m_DijkstraGrid[node->GetIndex()]);
		}
	}
	inline void FlowField::CalculateDirectionVectors()
	{
		
	}
	inline void FlowField::SetPathPoint(Elite::Vector2 pos)
	{
		auto idx = GetNodeIdxAtWorldPos(pos);
		if (IsNodeValid(idx))
		{
			GetNode(idx)->SetInfluence(0);
			CalculateDijkstra(idx);
		}
	}
	inline std::vector<int> FlowField::GetNeighbors(int idx)
	{
		std::vector<int> neighbors;
		const int columnStep = static_cast<int>(m_Nodes.size() / m_NrOfColumns); //Vertical
		constexpr int rowStep = 1; //Horizontal

		//Find top
		if (idx - columnStep >= 0) //Check if top not out of bounds
		{
			neighbors.push_back(idx - columnStep);
		}

		//Find bottom
		if (idx + columnStep < m_Nodes.size()) //Check if bottom not out of bounds
		{
			neighbors.push_back(idx + columnStep);
		}

		//Find left
		if ((idx - rowStep +1 )% columnStep != 0) //Check if left not out of bounds
		{
			neighbors.push_back(idx - rowStep);
		}

		//Find right
		if ((idx + rowStep) % columnStep != 0) //Check if right not out of bounds
		{
			neighbors.push_back(idx + rowStep);
		}

		return neighbors;
	}

	inline void FlowField::ToggleWall(Elite::Vector2 pos)
	{
		const int idx = GetNodeIdxAtWorldPos(pos);
		for (int i = 0; i < m_pWallIdx.size(); i++)
		{
			if (m_pWallIdx[i] == idx)
			{
				m_pWallIdx.erase(m_pWallIdx.begin() + i);
				m_pWalls.erase(m_pWalls.begin() + i);
				return;
			}
		}
		m_pWallIdx.push_back(idx);
		m_pWalls.push_back(std::make_unique<NavigationColliderElement> ( GetNodeWorldPos(idx), static_cast<float>(m_CellSize), static_cast<float>(m_CellSize) ));

		//auto idx = GetNodeIdxAtWorldPos(pos);
		//if (IsNodeValid(idx))
		//	GetNode(idx)->SetInfluence(influence);
	}

	inline void FlowField::OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged)
	{
		InitializeBuffer();
	}
}