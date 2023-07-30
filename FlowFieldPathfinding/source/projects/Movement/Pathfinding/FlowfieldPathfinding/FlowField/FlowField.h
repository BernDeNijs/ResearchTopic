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
			m_DirectionGrid = std::vector<Elite::Vector2>(m_Nodes.size());
			for (auto& node : m_Nodes)
			{
				node->SetInfluence(m_DijkstraGrid[node->GetIndex()]);
			}
		}
		void Render() const {}

		void SetPathPoint(Elite::Vector2 pos);
		void ToggleWall(Elite::Vector2 pos);
		Vector2 GetDirection(Vector2 pos) const { return GetDirection(GetNodeIdxAtWorldPos(pos)); }
		Vector2 GetDirection(int idx) const { return m_DirectionGrid[idx]; }

	protected:
		virtual void OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged) override;

	private:
		void CalculateDijkstra(int idx);
		void CalculateDirectionVectors();
		std::vector<int> GetNeighbors(int idx) const;
		std::vector<int> GetAllNeighbors(int idx) const;
		std::vector<int> m_DijkstraGrid;
		std::vector<Elite::Vector2> m_DirectionGrid;
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
		//Generate an empty grid of zero vectors
		std::fill(m_DirectionGrid.begin(), m_DirectionGrid.end(), Vector2{0,0});

		//For each grid square
		for (int idx = 0; idx < m_DirectionGrid.size(); idx++)
		{
			//skip walls
			if (m_DijkstraGrid[idx] == -1)
			{
				continue;
			}
			//ignore center
			if (m_DijkstraGrid[idx] == 1)
			{
				continue;
			}

			//Go through neighbors and find lowest value
			std::vector<int> neighbors = GetAllNeighbors(idx);
			int closestNeighbor{ -1 };
			int closestValue{ 99999 };
			for (const auto& neighbor : neighbors)
			{
				if (m_DijkstraGrid[neighbor] <= closestValue && m_DijkstraGrid[neighbor] != -1)
				{
					closestValue = m_DijkstraGrid[neighbor];
					closestNeighbor = neighbor;
				}
			}

			//If found
			if (closestNeighbor != -1)
			{
				//create vector pointing to closest neighbor
				const Vector2 currentPos{ GetNodeWorldPos(idx)};
				const Vector2 TargetPos{ GetNodeWorldPos(closestNeighbor)};
				const Vector2 directionVector = (TargetPos - currentPos).GetNormalized();
				m_DirectionGrid[idx] = directionVector;
			}
		}
	}
	inline void FlowField::SetPathPoint(Elite::Vector2 pos)
	{
		auto idx = GetNodeIdxAtWorldPos(pos);
		if (IsNodeValid(idx))
		{
			CalculateDijkstra(idx);
			CalculateDirectionVectors();
		}
	}
	inline std::vector<int> FlowField::GetNeighbors(int idx) const
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
	inline std::vector<int> FlowField::GetAllNeighbors(int idx) const
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
		if ((idx - rowStep + 1) % columnStep != 0) //Check if left not out of bounds
		{
			neighbors.push_back(idx - rowStep);
		}

		//Find right
		if ((idx + rowStep) % columnStep != 0) //Check if right not out of bounds
		{
			neighbors.push_back(idx + rowStep);
		}

		//Find topLeft
		if (idx - columnStep >= 0 && (idx - rowStep + 1) % columnStep != 0 ) //Check if top not out of bounds && Check if left not out of bounds
		{
			neighbors.push_back(idx - columnStep - rowStep);
		}

		//Find topRight
		if (idx - columnStep >= 0 && (idx + rowStep) % columnStep != 0) //Check if top not out of bounds && Check if right not out of bounds
		{
			neighbors.push_back(idx - columnStep + rowStep);
		}

		//Find bottomLeft
		if (idx + columnStep < m_Nodes.size() && (idx - rowStep + 1) % columnStep != 0) //Check if bottom not out of bounds && Check if left not out of bounds
		{
			neighbors.push_back(idx + columnStep - rowStep);
		}

		//Find bottomRight
		if (idx + columnStep < m_Nodes.size() && (idx + rowStep) % columnStep != 0) //Check if bottom not out of bounds && Check if left not out of bounds
		{
			neighbors.push_back(idx + columnStep + rowStep);
		}

		return neighbors;
	}

	inline void FlowField::ToggleWall(Elite::Vector2 pos)
	{
		InitializeBuffer();
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
	}

	inline void FlowField::OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged)
	{
		InitializeBuffer();
	}
}