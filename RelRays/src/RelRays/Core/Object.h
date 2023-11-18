#pragma once

#include <LoFox.h>

namespace RelRays {

	class World; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<World> origin);
	private:
		LoFox::Ref<World> m_Origin;
	};
}