#include "Timing.h"
#include <SDL/SDL.h>

namespace gameEngine {
	FpsLimiter::FpsLimiter(){

	}
	void FpsLimiter::init(float maxFPS){
		setMaxFPS(maxFPS);
	}

	void FpsLimiter::begin(){
		_startTicks = SDL_GetTicks();
	}

	float FpsLimiter::end(){
		calculateFPS();

		Uint32 frameTicks = SDL_GetTicks() - _startTicks;
		if (1000.0f / _maxFPS > frameTicks) {
			SDL_Delay((Uint32)(1000.0f / _maxFPS - frameTicks));
		}

		return _fps;
	}

	void FpsLimiter::setMaxFPS(float maxFPS){
		_maxFPS = maxFPS;
	}

	void FpsLimiter::calculateFPS() {
		static const int NUM_SAMPLES = 10;
		static float frameTimes[NUM_SAMPLES];
		static int currentFrame = 0;

		static Uint32 prevTicks = SDL_GetTicks();

		Uint32 currentTicks = SDL_GetTicks();

		_frameTime = currentTicks - prevTicks;
		frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

		prevTicks = currentTicks;

		int count;

		currentFrame++;
		if (currentFrame < NUM_SAMPLES)
			count = currentFrame;
		else
			count = NUM_SAMPLES;

		float frameTimeAvearge = 0;
		for (int i = 0; i < count; i++){
			frameTimeAvearge += frameTimes[i];
		}
		frameTimeAvearge /= count;

		if (frameTimeAvearge > 0) {
			_fps = 1000.0f / frameTimeAvearge;
		}
		else {
			_fps = 60.0f;
		}
	}
}