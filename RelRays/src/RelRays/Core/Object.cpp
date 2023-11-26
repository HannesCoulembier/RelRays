#include "RelRays/Core/Object.h"

#include "RelRays/Core/Environment.h"

namespace RelRays {

	Object::Object(LoFox::Ref<Environment> origin) {

		m_Origin = origin;
	}
}