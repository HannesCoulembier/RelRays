#include "Object.h"

#include "Environment.h"

namespace RelRays {

	Object::Object(LoFox::Ref<Environment> origin) {

		m_Origin = origin;
	}
}