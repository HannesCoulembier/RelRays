#pragma once

namespace RelRays {

	class Spectrum {

	public:
		Spectrum(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths, bool normalize = false);

		uint32_t GetWaveLengthCount() { return m_WaveLengthCount; }
		float GetMinWaveLength() { return m_MinWaveLength; }
		float GetMaxWaveLength() { return m_MaxWaveLength; }
		float GetWaveLengthRange() { return m_WaveLengthRange; }
		float* GetData() { return m_WaveLengths.data(); }
		std::vector<float> GetWaveLengths() { return m_WaveLengths; }

		static LoFox::Ref<Spectrum> Create(float minWaveLength, float maxWaveLength, const std::vector<float>& wavelengths, bool normalize = false);
	private:
		uint32_t m_WaveLengthCount;
		std::vector<float> m_WaveLengths;
		float m_MinWaveLength;
		float m_MaxWaveLength;
		float m_WaveLengthRange;
	};

	class ColorSpectra {

	public:
		ColorSpectra(LoFox::Ref<Spectrum> infrared, LoFox::Ref<Spectrum> red, LoFox::Ref<Spectrum> green, LoFox::Ref<Spectrum> blue, glm::vec4 relativeWeights);

		LoFox::Ref<Spectrum> GetInfraredColorSpectrum() { return m_Infrared; }
		LoFox::Ref<Spectrum> GetRedColorSpectrum() { return m_Red; }
		LoFox::Ref<Spectrum> GetGreenColorSpectrum() { return m_Green; }
		LoFox::Ref<Spectrum> GetBlueColorSpectrum() { return m_Blue; }
		uint32_t GetTotalWaveLengthCount() { return m_TotalWaveLengthCount; }
		glm::vec4 GetRelativeWeights() { return m_RelativeWeights; }

		static LoFox::Ref<ColorSpectra> Create(LoFox::Ref<Spectrum> infrared, LoFox::Ref<Spectrum> red, LoFox::Ref<Spectrum> green, LoFox::Ref<Spectrum> blue, glm::vec4 relativeWeights = glm::vec4(1.0f));
	private:
		LoFox::Ref<Spectrum> m_Infrared;
		LoFox::Ref<Spectrum> m_Red;
		LoFox::Ref<Spectrum> m_Green;
		LoFox::Ref<Spectrum> m_Blue;
		uint32_t m_TotalWaveLengthCount;
		glm::vec4 m_RelativeWeights;
	};
}