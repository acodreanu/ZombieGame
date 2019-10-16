#pragma once
#pragma once
#include <vector>
#include "ParticleBatch2D.h"
#include "SpriteBatch.h"

namespace gameEngine {

	class ParticleEngine2D {
	public:
		ParticleEngine2D();
		~ParticleEngine2D();

		void addParticleBatch(ParticleBatch2D* particleBatch);

		void update(float deltaTime);

		void draw(SpriteBatch* spriteBatch);

	private:
		std::vector<ParticleBatch2D*> m_batches;
	};
}

