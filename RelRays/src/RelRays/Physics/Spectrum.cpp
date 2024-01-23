#include "lfpch.h"
#include "RelRays/Physics/Spectrum.h"
#include "RelRays/Physics/Units.h"

namespace RelRays {

	Spectrum::Spectrum(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths, bool normalize) {

		m_MinWaveLength = minWaveLength;
		m_MaxWaveLength = maxWaveLength;
		m_WaveLengthRange = m_MaxWaveLength - m_MinWaveLength;
		m_WaveLengthCount = wavelengths.size();
		m_WaveLengths.resize(m_WaveLengthCount);
		if (normalize) {
			float spacing = ((maxWaveLength - minWaveLength) / Units::nm) / m_WaveLengthCount;
			float area = 0.0f;
			for (float wl : wavelengths)
				area += wl * spacing;
			if (area == 0.0f)
				m_WaveLengths = wavelengths;
			else
				for (uint32_t i = 0; i < m_WaveLengthCount; i++)
					m_WaveLengths[i] = wavelengths[i] / area;
		}
		else
			m_WaveLengths = wavelengths;
	}

	LoFox::Ref<Spectrum> Spectrum::Create(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths, bool normalize) {

		return LoFox::CreateRef<Spectrum>(minWaveLength, maxWaveLength, wavelengths, normalize);
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