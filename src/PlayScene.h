#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Background.h"
#include "PathNode.h"
#include "Asteroid.h"
#include "Heuristic.h"
#include "Scene.h"
#include "StarShip.h"
#include "Target.h"
#include "Tile.h"


class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

	//void moveStarShip() const;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	// Game Objects
	Target* m_pTarget;
	StarShip* m_pStarShip;
	Node* m_pNode;
	Background* m_pBackground;

	// Asteroids
	int m_roidNum = Config::MINE_NUM;
	std::vector<Asteroid*> m_pRoids;
	void m_buildRoids();
	void m_eraseRoids();
	void m_spawnRoids();

	std::vector<Node*> m_pNodes;
	void m_spawnNode();
	
	// UI Controls
	bool m_bToggleGrid;
	Label* m_pPlayInstructionsLabel{};
	Label* m_displayPathCostLabel{};
	

	// Pathfinding Objects and Functions
	
	std::vector<Tile*> m_pGrid; // Grid of Tile pointers
	bool m_isGridEnabled;
	
	void m_buildGrid();
	void m_computeTileCosts();

	void m_findShortestPath();
	void m_displayPathList();

	// void m_moveShipAlongPath();

	int m_spawnObject(NavigationObject* object);
	void m_spawnStarShip();
	void m_spawnTarget();

	
	void m_setGridEnabled(bool state);
	bool m_getGridEnabled() const;

	// convenience functions
	Tile* m_getTile(int col, int row);
	Tile* m_getTile(glm::vec2 grid_position);

	void m_resetGrid();

	// potential data structures
	 std::vector<Tile*> m_pOpenList;
	 std::vector<Tile*> m_pClosedList;
	 std::vector<Tile*> m_pPathList;

	 void m_moveShip();
	 int moveCounter = 0;
	 bool m_shipIsMoving = false;

};


#endif /* defined (__PLAY_SCENE__) */
