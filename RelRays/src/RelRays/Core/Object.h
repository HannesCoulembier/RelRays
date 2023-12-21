#pragma once

#include <LoFox.h>

#include "RelRays/Core/Material.h"
#include "RelRays/Core/Model.h"

namespace RelRays {

	class Environment; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<Environment> origin, glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model);
	private:
		LoFox::Ref<Environment> m_Origin;
		float m_ProperTime = 0.0f;
		float m_TimeOfCreation; // The time of creation relative to the origin

		// Temporary dev data
		glm::vec3 m_Pos;
		LoFox::Ref<Material> m_Material;
		LoFox::Ref<Model> m_Model;

		friend Environment;
	};
}