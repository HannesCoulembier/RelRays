#include "lfpch.h"
#include "RelRays/Core/Material.h"

#include "RelRays/Core/Defaults.h"

namespace RelRays {

	Material::Material(LoFox::Ref<Environment> origin, uint32_t name, glm::vec4 albedo, glm::vec4 emissionColor, float emissionStrength, float absorption) {

		m_Origin = origin;
		m_Name = name;

		m_Albedo = albedo;
		m_EmissionColor = emissionColor;
		m_EmissionStrength = emissionStrength;
		m_AbsorptionColorSpectra = ColorSpectra::Create(Defaults::AbsorptionSpectra::BlockPaint::Infrared, Defaults::AbsorptionSpectra::BlockPaint::Red, Defaults::AbsorptionSpectra::BlockPaint::Green, Defaults::AbsorptionSpectra::BlockPaint::Blue);
		m_EmissonColorSpectra = ColorSpectra::Create(Defaults::EmissionSpectra::BlockLED::Infrared, Defaults::EmissionSpectra::BlockLED::Red, Defaults::EmissionSpectra::BlockLED::Green, Defaults::EmissionSpectra::BlockLED::Blue);
		m_Absorption = absorption;
	}
}