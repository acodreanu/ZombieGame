#pragma once
#include <glm\glm.hpp>
#include <gameEngine\SpriteBatch.h>
#include "Agent.h"
#include "Human.h"
#include "Zombie.h"

class Human;
class Zombie;

const int BULLET_RADIUS = 5;

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed);
	~Bullet();

	glm::vec2 getPosition() const { return _position; };
	bool update(const std::vector<std::string>& levelData, float deltaTime);
	void draw(gameEngine::SpriteBatch& spriteBatch);
	bool collideWithAgent(Agent* agent);
	float getDamage() const { return _damage; }
private:

	bool collideWithWorld(const std::vector<std::string>& levelData);

	float _damage;
	glm::vec2 _position;
	glm::vec2 _direction;
	float _speed;
};

