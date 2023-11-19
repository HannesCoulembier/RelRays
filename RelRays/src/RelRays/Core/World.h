#pragma once

#include <LoFox.h>

#include "Object.h"

namespace RelRays {

	class World {

	public:
		World() {}
		void OnUpdate(float ts);
		void RenderFrame();
		void Destroy();

		LoFox::Ref<Object> CreateObject();

		static LoFox::Ref<World> Create();
	private:
		void SetSelf(LoFox::Ref<World> world);
		void Init();
	private:
		LoFox::Ref<World> m_Self;
		std::vector<LoFox::Ref<Object>> m_Objects;
	};
}