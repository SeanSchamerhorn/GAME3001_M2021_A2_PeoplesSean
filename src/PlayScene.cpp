#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include <iomanip>
#include <sstream>
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
}


void PlayScene::update()
{
	updateDisplayList();
	if(m_shipIsMoving)
	{
		m_moveShip();
	}
	
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		m_pStarShip->setEnabled(true);
		m_pTarget->setEnabled(true);
		m_pBackground->setEnabled(false);
		m_findShortestPath();
		m_displayPathList();
		SoundManager::Instance().setSoundVolume(50);
		SoundManager::Instance().playSound("start", 0);
	
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		if (!m_bToggleGrid)
		{
			m_setGridEnabled(true);
		}
	/*	else
		{
			m_bToggleGrid = false;
			m_setGridEnabled(m_bToggleGrid);
		}*/
		SoundManager::Instance().load("../Assets/audio/grid.mp3", "grid", SOUND_SFX);
		SoundManager::Instance().setSoundVolume(2);
		SoundManager::Instance().playSound("grid", 0);

	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_3))
	{
		m_pStarShip->setEnabled(true);
		m_pTarget->setEnabled(true);
		m_findShortestPath();
		SoundManager::Instance().load("../Assets/audio/engine_sound.mp3", "move", SOUND_SFX);
		SoundManager::Instance().setSoundVolume(30);
		SoundManager::Instance().playSound("move", 0);
		m_shipIsMoving = true;
		
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	// Add Background to Scene
	m_pBackground = new Background();
	m_pBackground->getTransform()->position = glm::vec2(0.0f, 0.0f);
	//addChild(m_pBackground);
	
	// Load music
	SoundManager::Instance().load("../Assets/audio/moonlight_sonata.mp3", "bgMusic", SOUND_MUSIC);
	SoundManager::Instance().playMusic("bgMusic", 0);
	SoundManager::Instance().setMusicVolume(7);

	
	// Setup the Grid
	m_buildGrid();
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	
	
	// Add Target to Scene
	m_pTarget = new Target();
	//m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset; // position in world space matches grid space
	//m_pTarget->setGridPosition(15, 11);
	//m_getTile(15, 11)->setTileStatus(GOAL);
	m_spawnTarget();
	addChild(m_pTarget);

	// Add StarShip to Scene
	m_pStarShip = new StarShip();
	//m_pStarShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset; // position in world space matches grid space
	//m_pStarShip->setGridPosition(1, 3);
	//m_getTile(1, 3)->setTileStatus(START);

	

	m_spawnStarShip();
	addChild(m_pStarShip);

	m_computeTileCosts();

	//m_buildRoids();
	//m_spawnRoids();

	// Add instruction Labels
	const SDL_Color red{ 255,0,0,255 };
	const SDL_Color white{ 255,255,255,255 };
	m_pPlayInstructionsLabel = new Label("Press ~ to open ImGui", "Consolas", 30, red, glm::vec2(400.0f, 50.0f));
	m_pPlayInstructionsLabel->setParent(this);
	addChild(m_pPlayInstructionsLabel);

	m_pPlayInstructionsLabel = new Label("Press 1 to Display Path", "Consolas", 15, red, glm::vec2(100.0f, 100.0f));
	m_pPlayInstructionsLabel->setParent(this);
	addChild(m_pPlayInstructionsLabel);


	m_pPlayInstructionsLabel = new Label("Press 2 to See Grid", "Consolas", 15, red, glm::vec2(300.0f, 100.0f));
	m_pPlayInstructionsLabel->setParent(this);
	addChild(m_pPlayInstructionsLabel);

	m_pPlayInstructionsLabel = new Label("Press 3 to Move Ship", "Consolas", 15, red, glm::vec2(500.0f, 100.0f));
	m_pPlayInstructionsLabel->setParent(this);
	addChild(m_pPlayInstructionsLabel);

	m_pPlayInstructionsLabel = new Label("Press 4 to Avoid", "Consolas", 15, red, glm::vec2(700.0f, 100.0f));
	m_pPlayInstructionsLabel->setParent(this);
	addChild(m_pPlayInstructionsLabel);
	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));

	m_displayPathCostLabel = new Label("Path Cost : ", "Consolas", 20,white,glm::vec2(70.0f,550.0f));
	m_displayPathCostLabel->setParent(this);
	addChild(m_displayPathCostLabel);
	//m_displayPathCostLabel->setText(total);
}

void PlayScene::GUI_Function()
{
	// TODO:
	// Toggle Visibility for the StarShip and the Target

	
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - M2021 - Lab 6", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	if(ImGui::Checkbox("Toggle Grid", &m_bToggleGrid))
	{
		// toggle grid on/off
		m_setGridEnabled(m_bToggleGrid);
	}

	ImGui::Separator();

	static int start_position[2] = { m_pStarShip->getGridPosition().x, m_pStarShip->getGridPosition().y };
	if(ImGui::SliderInt2("Start Position", start_position, 0, Config::COL_NUM - 1))
	{
		if(start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}

		m_getTile(m_pStarShip->getGridPosition())->setTileStatus(UNVISITED);
		m_pStarShip->getTransform()->position = m_getTile(start_position[0], start_position[1])->getTransform()->position + offset;
		m_pStarShip->setGridPosition(start_position[0], start_position[1]);
		m_getTile(m_pStarShip->getGridPosition())->setTileStatus(START);

		m_resetGrid();
	}

	ImGui::Separator();
	
	static int goal_position[2] = { m_pTarget->getGridPosition().x, m_pTarget->getGridPosition().y};
	if(ImGui::SliderInt2("Goal Position", goal_position, 0, Config::COL_NUM - 1))
	{
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}

		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(goal_position[0], goal_position[1])->getTransform()->position + offset;
		m_pTarget->setGridPosition(goal_position[0], goal_position[1]);
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);

		m_resetGrid();
		m_computeTileCosts();
	}

	ImGui::Separator();

	if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Move"))
	{
		m_shipIsMoving = true;
	}

	ImGui::Separator();

	if (ImGui::Button("Reset"))
	{
		m_resetGrid();
	}

	
	ImGui::End();
}

void PlayScene::m_buildRoids()
{
	for(int index = 0; index < m_roidNum; ++index)
	{
		auto roid = new Asteroid();
		addChild(roid);
		m_pRoids.push_back(roid);
		roid = nullptr;
	}
}

void PlayScene::m_eraseRoids()
{
	for (auto roid:m_pRoids)
	{
		removeChild(roid);
		delete roid;
		roid = nullptr;
	}

	m_pRoids.clear();
	m_pRoids.resize(0);
	m_pRoids.shrink_to_fit();
	
}

void PlayScene::m_spawnRoids()
{
	for(int index = 0; index < m_roidNum; ++index )
	{
		m_spawnObject(m_pRoids[index]);
		m_getTile(m_pRoids[index]->getGridPosition())->setTileStatus(IMPASSABLE);
	}
}



void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE; // alias for Tile size

	// add tiles to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile(); // create a new empty tile
			tile->getTransform()->position = glm::vec2(col * tileSize, row * tileSize); // world position
			tile->setGridPosition(col, row);
			addChild(tile);
			tile->addLabels();
			tile->setEnabled(false);
			m_pGrid.push_back(tile);
		}
	}

	// create references (connections) for each tile (node) to its neighbours (nodes)
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			// TopMost Row?
			if(row == 0)
			{
				tile->setNeighbourTile(TOP_TILE, nullptr);
			}
			else
			{
				// setup Top Neighbour
				tile->setNeighbourTile(TOP_TILE, m_getTile(col, row - 1));
			}
			
			// RightMost Col?
			if (col == Config::COL_NUM - 1)
			{
				tile->setNeighbourTile(RIGHT_TILE, nullptr);
			}
			else
			{
				// setup Right Neighbour
				tile->setNeighbourTile(RIGHT_TILE, m_getTile(col + 1, row));
			}

			// BottomMost Row?
			if (row == Config::ROW_NUM - 1)
			{
				tile->setNeighbourTile(BOTTOM_TILE, nullptr);
			}
			else
			{
				// Setup the Bottom Neighbour
				tile->setNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));
			}

			// LeftMost Col?
			if (col == 0)
			{
				tile->setNeighbourTile(LEFT_TILE, nullptr);
			}
			else
			{
				// Setup the Left Neighbour
				tile->setNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));
			}
		}
	}
}

void PlayScene::m_computeTileCosts()
{
	float dx = 0.0f;
	float dy = 0.0f;

	float f = 0.0f;
	float g = 0.0f;
	float h = 0.0f;
	
	for (auto tile : m_pGrid)
	{
		// Distance function
		g = Util::distance(tile->getGridPosition(), m_pTarget->getGridPosition());

		// Heuristic Calculation (Manhattan)
		dx = abs(tile->getGridPosition().x - m_pTarget->getGridPosition().x);
		dy = abs(tile->getGridPosition().y - m_pTarget->getGridPosition().y);
		h = dx + dy;

		// Pathfinding
		f = g + h;

		tile->setTileCost(f);
	}
	
}

void PlayScene::m_findShortestPath()
{
	if(m_pPathList.empty())
	{
		// Step 1 - Add Start position to open list
		auto startTile = m_getTile(m_pStarShip->getGridPosition());
		startTile->setTileStatus(OPEN);
		m_pOpenList.push_back(startTile);

		bool goalFound = false;

		// Step 2 - Loop until OpenList is empty or the Goal is found
		while(!m_pOpenList.empty() && !goalFound)
		{
			auto min = INFINITY;
			Tile* minTile; // temp tile pointer - initialized as nullptr
			int minTileIndex = 0;
			int count = 0; // count = the neighnour index

			std::vector<Tile*> neighbourList;
			for (int index = 0; index < NUM_OF_NEIGHBOUR_TILES; ++index)
			{
				if ((m_pOpenList[0]->getNeighbourTile(static_cast<NeighbourTile>(index)) != nullptr) && (m_pOpenList[0]->getNeighbourTile(static_cast<NeighbourTile>(index))->getTileStatus() !=IMPASSABLE))
				{
					neighbourList.push_back(m_pOpenList[0]->getNeighbourTile(static_cast<NeighbourTile>(index)));
				}

			}

			// for each neighbour Tile pointer in the neighbourList -> Find the Tile with the minimum tile cost and return it
			for (auto neighbour : neighbourList)
			{
				if (neighbour != nullptr)
				{
					// if the neighbour we are exploring is not the GOAL
					if (neighbour->getTileStatus() != GOAL)
					{
						if (neighbour->getTileCost() < min)
						{
							min = neighbour->getTileCost();
							minTile = neighbour;
							minTileIndex = count;
						}
						count++;
					}
					// else if it is the goal -> then exit
					else 
					{
						minTile = neighbour;
						m_pOpenList.push_back(minTile);
						goalFound = true;
						break;
					}
				}
			}


			// remove the reference of the current tile in the open list
			m_pPathList.push_back(m_pOpenList[0]);
			m_pOpenList.pop_back(); // empties open list
		
			// add the minTile to the openList
			m_pOpenList.push_back(minTile);
			minTile->setTileStatus(OPEN);
			neighbourList.erase(neighbourList.begin() + minTileIndex);

			// push all remaining neighbours onto the closed list
			for(auto neighbour : neighbourList)
			{
				if(neighbour->getTileStatus() == UNVISITED)
				{
					neighbour->setTileStatus(CLOSED);
					m_pClosedList.push_back(neighbour);
				}
			}	
		}
		m_pPathList.push_back(m_getTile(m_pTarget->getGridPosition()));
	}
	
	//m_displayPathList();
}

void PlayScene::m_displayPathList()
{
	std::cout << "- New Path ---------------------------------------" << std::endl;

	for (auto node : m_pPathList)
	{
		std::cout << "(" << node->getGridPosition().x << ", " << node->getGridPosition().y << ")" << std::endl;
	}

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Path Length: " << m_pPathList.size() << std::endl;
	std::cout << "--------------------------------------------------\n" << std::endl;

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	for (int index = 1; index < m_pPathList.size() -1; ++index)
	{
		m_pNode = new Node();
		m_pNode->getTransform()->position = m_pPathList[index]->getTransform()->position + offset;
		addChild(m_pNode);
	}
	auto total = 0.0f;
	for(int index = 0; index < m_pPathList.size(); ++index)
	{
		std::cout << "Tile Cost : " << m_getTile(m_pPathList[index]->getGridPosition())->getTileCost() << std::endl;
		total = m_pPathList[index]->getTileCost() + total;
	}
	std::cout << "Total Tile Cost : " << total << std::endl;

	// format string to 1 decimal place
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << total;
	const std::string total_string = stream.str();

	m_displayPathCostLabel->setText(total_string);
	
}

int PlayScene::m_spawnObject(NavigationObject* object)
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	Tile* randomTile = nullptr;
	auto randomTileIndex = 0;
	do
	{
		randomTileIndex = static_cast<int>(Util::RandomRange(0, m_pGrid.size() - 1));
		std::cout << randomTileIndex << std::endl;
		randomTile = m_pGrid[randomTileIndex];
	}
	while (randomTile->getTileStatus() != UNVISITED); // search for empty tile

	// if moving a tile that exists then set it's previous position to UNVISITED
	m_getTile(object->getGridPosition())->setTileStatus(UNVISITED);

	// move the texture to the random tile
	object->getTransform()->position = m_getTile(randomTile->getGridPosition())->getTransform()->position + offset;

	// set the grid position of the object
	object->setGridPosition(randomTile->getGridPosition().x, randomTile->getGridPosition().y);

	return randomTileIndex;
}

void PlayScene::m_spawnStarShip()
{
	m_spawnObject(m_pStarShip);
	m_getTile(m_pStarShip->getGridPosition())->setTileStatus(START);
	m_pStarShip->setVisible(true);
	
}

void PlayScene::m_spawnTarget()
{
	m_spawnObject(m_pTarget);
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
	
}

void PlayScene::m_setGridEnabled(const bool state)
{
	for (auto tile : m_pGrid)
	{
		tile->setEnabled(state);
		tile->setLabelsEnabled(state);
	}

	m_isGridEnabled = state;
	
}

bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

Tile* PlayScene::m_getTile(const int col, const int row)
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(glm::vec2 grid_position)
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;
	
	return m_pGrid[(row * Config::COL_NUM) + col];
}

void PlayScene::m_resetGrid()
{
	for(auto tile : m_pOpenList )
	{
		tile->setTileStatus(UNVISITED);
	}
	for (auto tile : m_pClosedList)
	{
		tile->setTileStatus(UNVISITED);
	}
	for(auto tile : m_pPathList)
	{
		tile->setTileStatus(UNVISITED);
	}

	m_pOpenList.clear();
	m_pClosedList.clear();
	m_pPathList.clear();

	// reset start tile
	m_getTile(m_pStarShip->getGridPosition())->setTileStatus(START);

	// reset Goal Tile
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
}

void PlayScene::m_moveShip()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	
	if (moveCounter < m_pPathList.size())
	{
		m_pStarShip->getTransform()->position = m_getTile(m_pPathList[moveCounter]->getGridPosition())->getTransform()->position + offset;
		m_pStarShip->setGridPosition(m_pPathList[moveCounter]->getGridPosition().x, m_pPathList[moveCounter]->getGridPosition().y);
		
		if (Game::Instance().getFrames() % 30 == 0)
		{
			moveCounter++;
		}
	}
	else
	{
		m_shipIsMoving = false;
	}
}