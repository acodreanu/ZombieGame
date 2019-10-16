#pragma once

#include <gameEngine\Window.h>
#include <gameEngine/GLSLProgram.h>
#include <gameEngine/Camera2D.h>
#include <gameEngine/InputManager.h>
#include <gameEngine/SpriteBatch.h>
#include <gameEngine\Timing.h>
#include <gameEngine/SpriteFont.h>
#include <gameEngine\AudioEngine.h>
#include <gameEngine\ParticleEngine2D.h>
#include <gameEngine\ParticleBatch2D.h>
#include "Level.h"
#include "Player.h"
#include "Zombie.h"
#include "Bullet.h"

enum class gameState { EXIT, PLAY };

class MainGame
{
public:
	MainGame();
	~MainGame();

	/// Runs the game
	void run();

private:
	/// Initializes the core systems
	void initSystems();

	/// Initializes the level and sets up everything
	void initLevel();

	/// Initializes the shaders
	void initShaders();

	/// Main game loop for the program
	void gameLoop();

	/// Handles input processing
	void processInput();

	/// Renders the game
	void drawGame();

	/// Draws the HUD
	void drawHud();

	/// Updates all agents
	void updateAgents(float deltaTime);

	/// Update all bullets
	void updateBullets(float deltaTime);

	/// Checks if player won
	void checkVictory();

	/// Adds blood to the particle engine
	void addBlood(const glm::vec2& position, int numParticles);

	/// Member Variables
	gameEngine::Window _window; ///< The game window

	gameEngine::GLSLProgram _textureProgram; ///< The shader program

	gameEngine::InputManager _inputManager; ///< Handles input

	gameEngine::Camera2D _camera; ///< Main Camera
	gameEngine::Camera2D _hudCamera; ///< Hud Camera

	gameEngine::SpriteBatch _agentSpriteBatch;
	gameEngine::SpriteBatch _hudSpriteBatch;

	gameEngine::ParticleEngine2D m_particleEngine;
	gameEngine::ParticleBatch2D* m_bloodParticleBatch;

	std::vector<Human*> _humans; ///<vector of all humans
	std::vector<Zombie*> _zombies; ///<vector of all zombies
	std::vector<Bullet> _bullets; ///<vector of bullets

	gameState _gameState;
	int _fps;
	std::vector<Level*> _levels;
	Player* _player;
	int _currentLevel;

	int _screenWidth;
	int _screenHeight;

	int _numHumansKilled; ///< Humans killed by player
	int _numZombiesKilled;

	gameEngine::AudioEngine m_audioEngine;

	gameEngine::SpriteFont* _spriteFont;
};

