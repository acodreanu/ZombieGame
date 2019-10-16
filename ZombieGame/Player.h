#pragma once

#include "Human.h"
#include <gameEngine\InputManager.h>
#include <gameEngine\Camera2D.h>
#include "Bullet.h"

class Gun;

class Player : public Human
{
public:
	Player();
	~Player();

	void init(float speed, glm::vec2 pos, gameEngine::InputManager* inputManager, gameEngine::Camera2D* camera, std::vector<Bullet>* bullets);
	void addGun(Gun* gun);
	void update(const std::vector <std::string>& levelData, std::vector<Human*>& humans, std::vector<Zombie*>& zombies, float deltaTime) override;
	void collideWithLevel(const std::vector<std::string>& levelData);
private:
	gameEngine::InputManager* _inputManager;
	int _currentGunIndex;
	std::vector<Gun*> _guns;
	gameEngine::Camera2D* _camera;
	std::vector<Bullet>* _bullets;
};

