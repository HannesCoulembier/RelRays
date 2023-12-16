#pragma once

#include <LoFox.h>

#include "RelRays/Physics/Spectrum.h"

namespace RelRays {

	class Environment; // Forward declaration

	class Material {

	public:
		Material(LoFox::Ref<Environment> origin, uint32_t name, glm::vec4 color, float metallic);
	private:
		LoFox::Ref<Environment> m_Origin;
		uint32_t m_Name;

		glm::vec4 m_Color;
		LoFox::Ref<ColorSpectra> m_ColorSpectra;
		float m_Metallic;

		friend Environment;
	};
}