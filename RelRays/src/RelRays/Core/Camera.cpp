#include "lfpch.h"
#include "RelRays/Core/Camera.h"

#include "RelRays/Core/Environment.h"

namespace RelRays {

	Camera::Camera(LoFox::Ref<Environment> origin, glm::vec3 pos, glm::vec3 viewDirection, Sensor sensor) {

		m_Origin = origin;
		m_TimeOfCreation = m_Origin->GetProperTime();

		m_StartPos = pos;
		m_StartVel = glm::vec3(0.0f, 0.0f, 0.0f) * (Units::m / Units::s);

		m_Pos = m_StartPos;
		m_Vel = m_StartVel;
		m_Acc = glm::vec3(0.0f);

		m_ViewDirection = viewDirection;

		m_Sensor = sensor;
	}

	void Camera::OnUpdate() {

		// TODO: make better when supporting accelerations
		m_Pos = m_StartPos + m_StartVel * (m_Origin->GetProperTime() - m_TimeOfCreation);
	}
}