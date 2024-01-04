#pragma once

#include "RelRays/Physics/Spectrum.h"

namespace RelRays {

	class Environment; // Forward declaration

	struct Sensor {

		LoFox::Ref<Spectrum> RedSpectrum;
		LoFox::Ref<Spectrum> GreenSpectrum;
		LoFox::Ref<Spectrum> BlueSpectrum;
		LoFox::Ref<ColorSpectra> ColorSpectra;
	};

	class Camera {

	public:
		Camera(LoFox::Ref<Environment> origin, glm::vec3 pos, glm::vec3 viewDirection, Sensor sensor);
	private:
		void OnUpdate(); // To be used by Environment
	private:
		LoFox::Ref<Environment> m_Origin;
		float m_ProperTime = 0.0f;
		float m_TimeOfCreation; // The time of creation relative to the origin

		glm::vec3 m_Pos = glm::vec3(0.0f);
		glm::vec3 m_Vel = glm::vec3(0.0f);
		glm::vec3 m_Acc = glm::vec3(0.0f);

		glm::vec3 m_ViewDirection = glm::vec3(0.0f);
		Sensor m_Sensor;

		friend Environment;
	};
}