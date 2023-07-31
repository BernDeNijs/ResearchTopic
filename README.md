# FlowField Pathfinding
 Research into flowfield pathfinding by Bern De Nijs.\
 Made for the course Digital Arts and Entertainment.\
 Made using the Elite Engine.

![Untitled](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/fa625a1e-41f7-4b4e-b8cd-356a537458d1)


## Introduction
Pathfinding is a fundamental problem in computer science and artificial intelligence, commonly encountered in various applications, including robotics, video games, and navigation systems. \
The goal of pathfinding is to find the shortest or most efficient path between two points in a given environment, considering obstacles, terrain costs, and other constraints.

Traditional pathfinding algorithms, like Dijkstra's algorithm and A* (A-star), are widely used and effective for finding optimal paths. \
These are a good and cost effective way when you're dealing with small amounts of agents but when dealing with groups of hundreds or thousands calculating each path individually becomes very expensive.\
So what do we do when we want large numbers of agents anyway?\
One solution would be Flowfield pathfinding.

### What is flowfield pathfinding?

Flowfield pathfinding is a technique used in artificial intelligence and game development for efficient pathfinding of agents in a dynamic environment.\
Instead of calculating a path for every agent we divide the field into a grid. \
For each cell of this grid we calculate a vector in the direction of the most efficient way to get to the destination, while avoiding static obstacles.\
By passing this information to the agents they can quickly navigate the game's world, avoiding obstacles, while minimizing the amount of computation required.

### Why use flowfield pathfinding?

Flowfield pathfinding offers several advantages over traditional pathfinding algorithms. It significantly reduces the computational cost of pathfinding for large crowds of agents, making it well-suited for real-time applications. 
Additionally, flowfield pathfinding enables agents to adapt to changing environments and dynamic obstacles efficiently.\
It provides smooth and natural-looking movement, enhancing the overall immersion and realism of the game.

## Implementation
### Create a grid
We start by dividing the world into tiles.
![GPP_CreateAGrid](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/ffa60716-a613-4d87-8f28-b813acc918c5)


### Fill it with obstacles (optional)
If we wish we can add some walls to our field. These will be impassable.
![GPP_AddObstacles](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/31168a2f-4662-431b-b848-35039eaf5389)


### Dijkstra floodfill
Next we need to determine the cost of every tile to the center. For this we use a Dijkstra alghoritm. \
This alghoritm fills our grid with values while taking into account the obstales we placed previously.

Steps:
1. Set the value of each tile to an impossible value like -1.
2. Set the value of each wall to another impossible value like -2.
3. Set the value of the destination node to 0 and add it to a deque of tiles that need to be visited.
4. While that double-ended queue isn't empty we find the neighbors (horizontal and vertical) of the first element in the deque that aren't walls.
5. We calculate their cost and add them to the deque. Then we pop the fist element out of the deque.
6. We repeat this proces until every tile has a value.

![GPP_DijkstraFloodFill](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/ce4c6752-0086-456e-9b6b-38ab47a620fd)


### Flow field
The next step is to use our new values to generate a flowfield. This process is reasonably simple.

Steps:
1. Fill a vector or grid with zero vectors.
2. Loop over every tile.
3. Ignore it if it's a wall, the destination or a tile that's out of reach.
4. Find all it's neighbours (this time also diagonally) and find the one with the lowest cost.
5. Calculate a vector from the current tile to it's lowest cost neighbor.
6. Save this vector.
7. Repeat until we went over every tile.

![GPP_FlowField](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/82c34e58-81e9-461f-8a56-7f569e3db7d2)


### Agents
Now that we have our flowfield we just need our agents to get the vector of the tile they're currently in and use that for their steering.

## Personal additions & changes
### Values
Most of the sources i found decided to assign large values to walls & unsearched tiles. 
I found this unsafe.
If a grid is large enough to have the cost of the tiles actually reach those values the flowfield's vectors change in directions it shouldn't. This is why i used negative numbers to ensure this couldn't happen.

### Corners
Another problem i encountered was with corners. If the walls placed were one continuous line everything works as expected but the moment you have 2 walls diagonaly on the grid the flowfield assumes it can go in between them. Something the agents can't actually do.
This is why i added an additional check for these corners to be ignored during the generation of the flowfield.

![CornerError](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/7301a398-0a33-4ea0-bd6b-8a8d52592fc6)
![CornerFixed](https://github.com/BernDeNijs/ResearchTopic/assets/114000661/2030bae4-5e42-45f9-ae39-7d069b369909)


## Conclusion

Flowfield pathfinding is a powerful and effective method for navigation in games that use large numbers of agents. Generating the flowfield seperately, the pathfinding process becomes significantly faster and able to handle a large number of agents without sacrificing a lot of performance. The technique's ability to adapt to changing environments and handle walls and corners makes it a valuable tool.

## Sources
http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdf
https://howtorts.github.io/2014/01/04/basic-flow-fields.html
https://howtorts.github.io/2013/12/31/generating-a-path-dijkstra.html
https://gamedevelopment.tutsplus.com/understanding-goal-based-vector-field-pathfinding--gamedev-9007t
