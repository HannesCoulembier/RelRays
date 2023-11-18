#include "Object.h"

#include "World.h"

namespace RelRays {

	Object::Object(LoFox::Ref<World> origin) {

		m_Origin = origin;
	}
}