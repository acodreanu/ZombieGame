#include "MainGame.h"

#include <gameEngine/gameEngine.h>
#include <gameEngine\ResourceManager.h>
#include <gameEngine\Timing.h>
#include <random>
#include <ctime>
#include <algorithm>
#include <gameEngine/EngineErrors.h>
#include "Gun.h"
#include <glm\gtx\rotate_vector.hpp>

#include <glm\glm.hpp>

#include <SDL/SDL.h>
#include <iostream>

const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;
const float PLAYER_SPEED = 5.0f;

MainGame::MainGame() : _screenHeight(768), _screenWidth(1024), _gameState(gameState::PLAY), _fps(0), _player(nullptr), _numHumansKilled(0), _numZombiesKilled(0) {
	_camera.init(_screenWidth, _screenHeight);
}

MainGame::~MainGame() {
	for (int i = 0; i < _levels.size(); i++){
		delete _levels[i];
	}
}

void MainGame::run() {
	initSystems();
	initLevel();

	gameEngine::Music music = m_audioEngine.loadMusic("Sound/XYZ.ogg");
	music.play(-1);

	gameLoop();
}

void updateBloodParticle(gameEngine::Particle2D& p, float deltaTime) {
	p.position += p.velocity * deltaTime;
	p.color.a = (GLubyte)(p.life * 255.0f);
}

void MainGame::initSystems() {
	gameEngine::init();

	// Initialize sound, must happen after gameEngine::init
	m_audioEngine.init();

	// Create window
	_window.create("Zombie Game", _screenWidth, _screenHeight, 0);

	// Background
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	// Set up the shaders
	initShaders();

	// Initialize the spirtebatch
	_agentSpriteBatch.init();
	_hudSpriteBatch.init();

	// Initialize sprite font
	_spriteFont = new gameEngine::SpriteFont("Fonts/Sans.ttf", 64);

	// Set up camera
	_camera.init(_screenWidth, _screenHeight);
	_hudCamera.init(_screenWidth, _screenHeight);
	_hudCamera.setPosition(glm::vec2(_screenWidth / 2, _screenHeight / 2));

	// Initialize particles
	m_bloodParticleBatch = new gameEngine::ParticleBatch2D();
	m_bloodParticleBatch->init(1000, 0.05f,
								gameEngine::ResourceManager::getTexture("Textures/particle.png"),
								[](gameEngine::Particle2D& p, float deltaTime) {
		p.position += p.velocity * deltaTime;
		p.color.a = (GLubyte)(p.life * 255.0f);
	});
	m_particleEngine.addParticleBatch(m_bloodParticleBatch);
}

void MainGame::initLevel(){
	// Level 1
	_levels.push_back(new Level("Levels/level1.txt"));
	_currentLevel = 0;

	_player = new Player();
	std::cout << "main: " << _bullets.size() << " " << &_bullets << std::endl;
	_player->init(PLAYER_SPEED, _levels[_currentLevel]->getStartPlyaerPos(), &_inputManager, &_camera, &_bullets);

	_humans.push_back(_player);


	std::mt19937 randomEngine;
	randomEngine.seed(time(nullptr));
	std::uniform_int_distribution<int> randX(2, _levels[_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randY(2, _levels[_currentLevel]->getHeight() - 2);


	// Add all the random humans
	for (int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++) {
		_humans.push_back(new Human);
		glm::vec2 pos(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
		_humans.back()->init(HUMAN_SPEED, pos);
	}

	// Add the zombies
	const std::vector<glm::vec2>& zombiePositions = _levels[_currentLevel]->getZombieStartPositions();
	for (int i = 0; i < zombiePositions.size(); i++) {
		_zombies.push_back(new Zombie);
		_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
	}

	// Set up the player guns
	const float BULLET_SPEED = 20.0f;
	_player->addGun(new Gun("Magnum", 10, 1, 5.0f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
	_player->addGun(new Gun("Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/shotgun.wav")));
	_player->addGun(new Gun("MP5", 2, 1, 10.0f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect("Sound/shots/cg1.wav")));
}

void MainGame::initShaders() {
    // Compile our color shader
    _textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    _textureProgram.addAttribute("vertexPosition");
    _textureProgram.addAttribute("vertexColor");
    _textureProgram.addAttribute("vertexUV");
    _textureProgram.linkShaders();
}

void MainGame::gameLoop() {

	const float DESIRED_FPS = 60.0f;
	const int MAX_PHYSICS_STEPS = 6;

	gameEngine::FpsLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(60.0f);
	std::cout << "Author: Blazzer Bitches\n\n";

	const float CAMERA_SCALE = 1.0f / 2.0f;
	_camera.setScale(CAMERA_SCALE);

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float previousTicks = SDL_GetTicks();

	while (_gameState == gameState::PLAY){
		fpsLimiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		_inputManager.update();

		processInput();

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS) {
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateAgents(deltaTime);
			updateBullets(deltaTime);
			m_particleEngine.update(deltaTime);
			totalDeltaTime -= deltaTime;
			i++;
		}


		_camera.setPosition(_player->getPosition());
		_camera.update();
		_hudCamera.update();

		drawGame();

		_fps = fpsLimiter.end();
		std::cout << _fps << std::endl;
	}
}

void MainGame::updateAgents(float deltaTime) {
	// update all humans
	for (int i = 0; i < _humans.size(); i++){
		_humans[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	// Update all zombies
	for (int i = 0; i < _zombies.size(); i++) {
		_zombies[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	// Update zombie collisions
	for (int i = 0; i < _zombies.size() - 1; i++) {
		// Collide with other zombies
		for (int j = i + 1; j < _zombies.size(); j++) {
			_zombies[i]->collideWithAgent(_zombies[j]);
		}
		// Collide with human
		for (int j = 1; j < _humans.size(); j++) {
			if (_zombies[i]->collideWithAgent(_humans[j])) {
				// Add the new zombie
				_zombies.push_back(new Zombie);
				_zombies.back()->init(ZOMBIE_SPEED, _humans[j]->getPosition());
				// Delete the human
				delete _humans[j];
				_humans[j] = _humans.back();
				_humans.pop_back();
			}
		}

		// Collide with player
		if (_zombies[i]->collideWithAgent(_player)) {
			gameEngine::fatalError("YOU LOSE!");
		}
	}

	// Update human collisions
	for (int i = 0; i < _humans.size() - 1; i++) {
		// Collide with other humans
		for (int j = i + 1; j < _humans.size(); j++) {
			_humans[i]->collideWithAgent(_humans[j]);
		}
	}
}

void MainGame::updateBullets(float deltaTime) {
	// Update and collide with world
	for (int i = 0; i < _bullets.size();) {
		if (_bullets[i].update(_levels[_currentLevel]->getLevelData(), deltaTime)) {
			// The bullet collided with a wall
			_bullets[i] = _bullets.back();
			_bullets.pop_back();
		}
 else
		i++;
	}

	bool wasBulletRemoved;
	// Collide with humans
	for (int i = 0; i < _bullets.size(); i++) {
		wasBulletRemoved = false;
		// Loop through zombies
		for (int j = 0; j < _zombies.size();) {
			// Check collision
			if (_bullets[i].collideWithAgent(_zombies[j])) {
				// Add blood
				addBlood(_bullets[i].getPosition(), 5);
				// Damage the zombie, and kill if its out of health
				if (_zombies[j]->applyDmage(_bullets[i].getDamage())){
					// If the zombie died, delete him
					delete _zombies[j];
					_zombies[j] = _zombies.back();
					_zombies.pop_back();
					_numZombiesKilled++;
				} else {
					j++;
				}
				// Remove the bullet
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				wasBulletRemoved = true;
				i--; // Make sure we don't skip a bullet
				break;
			} else {
				j++;
			}
		}

		// Loop through humans
		if (wasBulletRemoved == false) {
			for (int j = 1; j < _humans.size();) {
				// Check collision
				if (_bullets[i].collideWithAgent(_humans[j])) {
					addBlood(_bullets[i].getPosition(), 5);
					// Damage the human, and kill if its out of health
					if (_humans[j]->applyDmage(_bullets[i].getDamage())){
						// If the human died, delete him
						delete _humans[j];
						_humans[j] = _humans.back();
						_humans.pop_back();
						_numHumansKilled++;
					}
					else {
						j++;
					}
					// Remove the bullet
					_bullets[i] = _bullets.back();
					_bullets.pop_back();
					wasBulletRemoved = true;
					i--; // Make sure we don't skip a bullet
					break;
				}
				else {
					j++;
				}
			}
		}
	}
}

void MainGame::checkVictory() {
	if (_zombies.empty()) {
		// TODO: Support for multiple levels
		std::printf("*** You Win! ***\nYou killed %d humans and %d zombies. There are %d/%d humans remaining\n", _numHumansKilled, _numZombiesKilled, _humans.size() - 1, _levels[_currentLevel]->getNumHumans());
		gameEngine::fatalError("");
	}
}

void MainGame::processInput() {
    SDL_Event evnt;
    //Will keep looping until there are no more events to process
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
            case SDL_QUIT:
				_gameState = gameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                _inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
                break;
            case SDL_KEYDOWN:
                _inputManager.pressKey(evnt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                _inputManager.releaseKey(evnt.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                _inputManager.pressKey(evnt.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                _inputManager.releaseKey(evnt.button.button);
                break;
        }
    }
}

void MainGame::drawGame() {
    // Set the base depth to 1.0
    glClearDepth(1.0);
    // Clear the color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_textureProgram.use();

	glActiveTexture(GL_TEXTURE0);

	// make sure the texture is using 
	GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// grap the camera matrix
	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	// Draw level
	_levels[_currentLevel]->draw();

	_agentSpriteBatch.begin();

	const glm::vec2 agentDims(AGENT_RADIUS * 2.0f);

	// Draw the humans
	for (int i = 0; i < _humans.size(); i++){
		if (_camera.isBoxInView(_humans[i]->getPosition(), agentDims))
		_humans[i]->draw(_agentSpriteBatch);
	}

	// Draw the zombies
	for (int i = 0; i < _zombies.size(); i++){
		if (_camera.isBoxInView(_zombies[i]->getPosition(), agentDims))
		_zombies[i]->draw(_agentSpriteBatch);
	}

	// Draw the bullets
	for (int i = 0; i < _bullets.size(); i++) {
		_bullets[i].draw(_agentSpriteBatch);
	}
	_agentSpriteBatch.end();

	_agentSpriteBatch.renderBatch();

	// Render the particles
	m_particleEngine.draw(&_agentSpriteBatch);

	drawHud();

	_textureProgram.unuse();

    // Swap our buffer and draw everything to the screen!
    _window.swapBuffer();
}

void MainGame::drawHud(){
	char buffer[256];

	glm::mat4 projectionMatrix = _hudCamera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	_hudSpriteBatch.begin();

	sprintf_s(buffer, "Num Humans %d", _humans.size());
	_spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0, 0), glm::vec2(0.5f), 0.0f, gameEngine::ColorRGBA8(255, 255, 255, 255));

	sprintf_s(buffer, "Num Zombies %d", _zombies.size());
	_spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0, 36), glm::vec2(0.5f), 0.0f, gameEngine::ColorRGBA8(255, 255, 255, 255));

	_hudSpriteBatch.end();
	_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2& position, int numParticles) {
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 360.0f);

	glm::vec2 vel(2.0f, 0.0f);
	gameEngine::ColorRGBA8 col(255, 0, 0, 255);
	for (int i = 0; i < numParticles; i++) {
		m_bloodParticleBatch->addParticle(position, glm::rotate(vel, randAngle(randEngine)), col, 30.0f);
	}
}