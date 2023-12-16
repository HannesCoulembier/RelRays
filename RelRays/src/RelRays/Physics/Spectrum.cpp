#include "lfpch.h"
#include "RelRays/Physics/Spectrum.h"

namespace RelRays {

	Spectrum::Spectrum(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths) {

		m_MinWaveLength = minWaveLength;
		m_MaxWaveLength = maxWaveLength;
		m_WaveLengthRange = m_MaxWaveLength - m_MinWaveLength;
		m_WaveLengthCount = wavelengths.size();
		m_WaveLengths.resize(m_WaveLengthCount);
		m_WaveLengths = wavelengths;
	}

	LoFox::Ref<Spectrum> Spectrum::Create(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths) {

		return LoFox::CreateRef<Spectrum>(minWaveLength, maxWaveLength, wavelengths);
	}

	ColorSpectra::ColorSpectra(LoFox::Ref<Spectrum> infrared, LoFox::Ref<Spectrum> red, LoFox::Ref<Spectrum> green, LoFox::Ref<Spectrum> blue, glm::vec4 relativeWeights) {

		m_Infrared = infrared;
		m_Red = red;
		m_Green = green;
		m_Blue = blue;
		m_TotalWaveLengthCount = m_Infrared->GetWaveLengthCount() + m_Red->GetWaveLengthCount() + m_Green->GetWaveLengthCount() + m_Blue->GetWaveLengthCount();
		m_RelativeWeights = relativeWeights;
	}

	LoFox::Ref<ColorSpectra> ColorSpectra::Create(LoFox::Ref<Spectrum> infrared, LoFox::Ref<Spectrum> red, LoFox::Ref<Spectrum> green, LoFox::Ref<Spectrum> blue, glm::vec4 relativeWeights) {

		return LoFox::CreateRef<ColorSpectra>(infrared, red, green, blue, relativeWeights);
	}
}