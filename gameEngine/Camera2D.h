#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gameEngine {

	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		void init(int screenWidth, int screenHeight);
		void update();
		void setPosition(glm::vec2& newPosition) { _needsMatrixUpdate = true; _position = newPosition; }
		void setScale(float newScale) { _needsMatrixUpdate = true; _scale = newScale; }

		glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);

		bool isBoxInView(const glm::vec2& position, const glm::vec2& dimensions);

		float getScale() { return _scale; }
		glm::vec2 getPosition() { return _position; }
		glm::mat4 getCameraMatrix() { return _cameraMatrix; }
	private:
		int _screenWidth, _screenHeight;
		bool _needsMatrixUpdate;
		glm::vec2 _position;
		glm::mat4 _cameraMatrix;
		float _scale;
		glm::mat4 _orthoMatrix;
	};
}

