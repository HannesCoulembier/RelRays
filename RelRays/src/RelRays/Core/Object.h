#pragma once

#include <LoFox.h>

#include "RelRays/Core/Material.h"

namespace RelRays {

	class Environment; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<Environment> origin, glm::vec3 pos, float radius, LoFox::Ref<Material> material);
	private:
		LoFox::Ref<Environment> m_Origin;
		float m_ProperTime = 0.0f;
		float m_TimeOfCreation; // The time of creation relative to the origin

		// Temporary dev data
		glm::vec3 m_Pos;
		float m_Radius;
		LoFox::Ref<Material> m_Material;

		friend Environment;
	};
}