#pragma once

#include <string>
#include <vector>

#include <gameEngine\SpriteBatch.h>
#include <gameEngine\ResourceManager.h>

const int TILE_WIDTH = 64;

class Level
{
public:
	// Load the lever
	Level(const std::string& fileName);
	~Level();

	void draw();

	// getters
	glm::vec2 getStartPlyaerPos() const { return _startPlayerPos; }
	const std::vector<glm::vec2>& getZombieStartPositions() const { return _zombieStartPositions; }
	const std::vector<std::string>& getLevelData() const { return _levelData; }
	int getNumHumans() const { return _numHumans; }
	int getWidth() const { return _levelData[0].size(); }
	int getHeight() const { return _levelData.size(); }
private:
	std::vector<std::string> _levelData;
	int _numHumans;
	gameEngine::SpriteBatch _spriteBatch;

	glm::vec2 _startPlayerPos;
	std::vector<glm::vec2> _zombieStartPositions;
};

