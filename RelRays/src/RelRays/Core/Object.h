#pragma once

#include <LoFox.h>

namespace RelRays {

	class Environment; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<Environment> origin);
	private:
		LoFox::Ref<Environment> m_Origin;
	};
}