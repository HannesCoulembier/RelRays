#pragma once

#include <LoFox.h>

namespace RelRays {

	class Environment; // Forward declaration

	class Material {

	public:
		Material(LoFox::Ref<Environment> origin, uint32_t name, glm::vec3 color, float metallic);
	private:
		LoFox::Ref<Environment> m_Origin;
		uint32_t m_Name;

		glm::vec3 m_Color;
		float m_Metallic;

		friend Environment;
	};
}