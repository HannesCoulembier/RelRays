#include "lfpch.h"
#include "RelRays/Core/Material.h"

namespace RelRays {

	Material::Material(LoFox::Ref<Environment> origin, uint32_t name, glm::vec3 color, float metallic) {

		m_Origin = origin;
		m_Name = name;

		m_Color = color;
		m_Metallic = metallic;
	}
}