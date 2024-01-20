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
		m_AbsorptionColorSpectra = ColorSpectra::Create(Defaults::AbsorptionSpectra::FakePaint::Infrared, Defaults::AbsorptionSpectra::FakePaint::Red, Defaults::AbsorptionSpectra::FakePaint::Green, Defaults::AbsorptionSpectra::FakePaint::Blue);
		m_EmissonColorSpectra = ColorSpectra::Create(Defaults::EmissionSpectra::LED::Infrared, Defaults::EmissionSpectra::LED::Red, Defaults::EmissionSpectra::LED::Green, Defaults::EmissionSpectra::LED::Blue);
		m_Absorption = absorption;
	}
}