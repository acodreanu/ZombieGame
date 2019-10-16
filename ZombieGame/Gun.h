#pragma once
#include <string>
#include <glm\glm.hpp>
#include <vector>
#include "Bullet.h"
#include <gameEngine\AudioEngine.h>

class Gun
{
public:
	Gun(std::string name, int fireRate, int bulletsPerShot, float spread, float bulletDamage, float bulletSpeed, gameEngine::SoundEffect fireEffect);
	~Gun();

	void update(bool isMouseDown, const glm::vec2 position, glm::vec2 direction, std::vector<Bullet>& bullets, float deltaTime);
private:

	gameEngine::SoundEffect m_fireEffect;

	void fire(glm::vec2 direction, const glm::vec2 position, std::vector<Bullet>& bullets);

	std::string _name;

	int _fireRate; ///< Fire rate in terms of frames

	int _bulletsPerShot; ///< How many bullets are fired at a time

	float _spread; ///< Accuracy

	float _bulletSpeed;
	float _bulletDamage;

	float _frameCounter;
};

