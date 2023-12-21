#include "lfpch.h"
#include "RelRays/Core/Material.h"

#include "RelRays/Core/Defaults.h"

namespace RelRays {

	Material::Material(LoFox::Ref<Environment> origin, uint32_t name, glm::vec4 albedo, glm::vec4 emissionColor, float emissionStrength, float metallic) {

		m_Origin = origin;
		m_Name = name;

		m_Albedo = albedo;
		m_EmissionColor = emissionColor;
		m_EmissionStrength = emissionStrength;
		m_ColorSpectra = ColorSpectra::Create(Defaults::Colors::InfraredSpectrum, Defaults::Colors::RedSpectrum, Defaults::Colors::GreenSpectrum, Defaults::Colors::BlueSpectrum);
		m_Metallic = metallic;
	}
}