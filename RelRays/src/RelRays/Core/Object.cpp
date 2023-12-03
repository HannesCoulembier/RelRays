#include "RelRays/Core/Object.h"

#include "RelRays/Core/Environment.h"

namespace RelRays {

	Object::Object(LoFox::Ref<Environment> origin, glm::vec3 pos, float radius, LoFox::Ref<Material> material) {

		m_Origin = origin;
		m_TimeOfCreation = m_Origin->GetProperTime();
		m_Pos = pos;
		m_Radius = radius;
		m_Material = material;
	}
}