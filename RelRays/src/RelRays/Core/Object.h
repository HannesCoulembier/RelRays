#pragma once

#include <LoFox.h>

namespace RelRays {

	class Environment; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<Environment> origin, glm::vec3 pos, float radius);
	private:
		LoFox::Ref<Environment> m_Origin;
		float m_ProperTime = 0.0f;
		float m_TimeOfCreation; // The time of creation relative to the origin

		// Temporary dev data
		glm::vec3 m_Pos;
		float m_Radius;

		friend Environment;
	};
}