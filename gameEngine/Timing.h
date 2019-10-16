#pragma once

namespace gameEngine {
	class FpsLimiter {
	public:
		FpsLimiter();
		void init(float maxFPS);

		void begin();
		// return the current fps
		float end();
		void setMaxFPS(float maxFPS);

	private:
		void calculateFPS();

		float _targetFPS;
		unsigned int _startTicks;

		float _fps;
		float _frameTime;
		float _maxFPS;
	};
}