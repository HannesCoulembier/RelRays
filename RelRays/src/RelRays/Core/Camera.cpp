#include "lfpch.h"
#include "RelRays/Core/Camera.h"

#include "RelRays/Core/Environment.h"

namespace RelRays {

	Camera::Camera(LoFox::Ref<Environment> origin, glm::vec3 pos, glm::vec3 viewDirection, Sensor sensor) {

		m_Origin = origin;
		m_TimeOfCreation = m_Origin->GetProperTime();

		m_Pos = pos;
		m_Vel = glm::vec3(0.0f);
		m_Acc = glm::vec3(0.0f);

		m_ViewDirection = viewDirection;

		m_Sensor = sensor;
	}

	void Camera::OnUpdate() {


	}
}