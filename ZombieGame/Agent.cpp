#include "Agent.h"
#include <gameEngine\ResourceManager.h>
#include "Level.h"
#include <algorithm>

Agent::Agent()
{
}


Agent::~Agent()
{
}

bool Agent::collideWithLevel(const std::vector<std::string>& levelData) {

	std::vector<glm::vec2> collideTilePosition;

	// Check the four corners
	// First corner
	checkTilePosition(levelData, collideTilePosition, _position.x, _position.y);
	// Second corner
	checkTilePosition(levelData, collideTilePosition, _position.x + AGENT_WIDTH, _position.y);
	// Third corner
	checkTilePosition(levelData, collideTilePosition, _position.x, _position.y + AGENT_WIDTH);
	// Forth corner
	checkTilePosition(levelData, collideTilePosition, _position.x + AGENT_WIDTH, _position.y + AGENT_WIDTH);
	// Do the collision
	if (collideTilePosition.size() == 0)
		return false;

	for (auto i = 0; i < collideTilePosition.size(); i++) {
		collideWithTile(collideTilePosition[i]);
	}
	return true;
}

bool Agent::collideWithAgent(Agent* agent){

	const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

	glm::vec2 centerPosA = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0) {

		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		_position += collisionDepthVec / 2.0f;
		agent->_position -= collisionDepthVec / 2.0f;
		return true;
	}
	return false;
}

bool Agent::applyDmage(float damage) {
	_health -= damage;
	if (_health <= 0)
		return true;
	return false;
}

void Agent::draw(gameEngine::SpriteBatch& _spriteBatch) {

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	glm::vec4 destRect;
	destRect.x = _position.x;
	destRect.y = _position.y;
	destRect.z = AGENT_WIDTH;
	destRect.w = AGENT_WIDTH;
	_spriteBatch.draw(destRect, uvRect, m_textureID, 0.0f, _color, m_direction);
}

void Agent::checkTilePosition(const std::vector<std::string>& levelData, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
	glm::vec2 cornerPos = glm::vec2(floor(x / (float)TILE_WIDTH), floor(y / (float)TILE_WIDTH));

	// If we are outside the world, return
	if (cornerPos.x < 0 || cornerPos.x >= levelData[0].length() ||
		cornerPos.y < 0 || cornerPos.y >= levelData.size()) {
		return;
	}
	if (levelData[cornerPos.y][cornerPos.x] != '.') {
		collideTilePositions.push_back(cornerPos * (float)TILE_WIDTH + glm::vec2((float)(TILE_WIDTH / 2.0f)));
	}
}

// AABB collision
void Agent::collideWithTile(glm::vec2 tilePos) {

	const float TILE_RADIUS = TILE_WIDTH / 2.0f;
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

	glm::vec2 centerPlayerPos = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 distVec = centerPlayerPos - tilePos;

	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE - abs(distVec.y);

	// We are colliding
	if (xDepth > 0 || yDepth > 0) {
		if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f)) {
			if (distVec.x < 0)
				_position.x -= xDepth;
			else
				_position.x += xDepth;
		}
		else {
			if (distVec.y < 0)
				_position.y -= yDepth;
			else
				_position.y += yDepth;
		}
	}
}