#include "RelRays/Core/Object.h"

#include "RelRays/Core/Environment.h"

namespace RelRays {

	Object::ObjectFragment::ObjectFragment(glm::vec4 pos, glm::vec3 vel, LoFox::Ref<Material> material)
		: m_StartPos(pos), m_StartVel(vel), m_Material(material) {}

	Object::Object(LoFox::Ref<Environment> origin, glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model) {

		m_Origin = origin;
		m_TimeOfCreation = m_Origin->GetProperTime();

		ObjectFragment firstFragment = ObjectFragment(glm::vec4(pos, m_TimeOfCreation), glm::vec3(0.0f), material);
		m_Fragments.push_back(firstFragment);

		m_Model = model;
	}

	void Object::SetPos(glm::vec3 pos) {

		ObjectFragment latestFragment = GetLatestFragment();
		AddEvent(glm::vec4(pos, m_Origin->GetProperTime()), latestFragment.m_StartVel, latestFragment.m_Material);
	}

	void Object::SetVel(glm::vec3 vel) {

		ObjectFragment latestFragment = GetLatestFragment();
		AddEvent(Get4Pos(m_Origin->GetProperTime()), vel, latestFragment.m_Material);
	}

	void Object::SetState(glm::vec3 pos, glm::vec3 vel) {

		AddEvent(glm::vec4(pos, m_Origin->GetProperTime()), vel, GetLatestFragment().m_Material);
	}

	void Object::AddEvent(glm::vec4 pos, glm::vec3 vel, LoFox::Ref<Material> material) {

		ObjectFragment latestFragment = GetLatestFragment();
		
		if (pos == latestFragment.m_StartPos && vel == latestFragment.m_StartVel && material == latestFragment.m_Material)
			return; // If both fragments are exactly the same, do nothing

		float latestFragmentCreationTime = latestFragment.m_StartPos.w;
		LF_CORE_ASSERT(pos.w >= latestFragmentCreationTime, "History is already defined for this moment!"); // Events can only be inserted or changed at the end of the history

		if (pos.w == latestFragmentCreationTime) { // Change the fragment already active (as the times are the same)

			m_Fragments[m_Fragments.size() - 1].m_StartPos = pos;
			m_Fragments[m_Fragments.size() - 1].m_StartVel = vel;
			m_Fragments[m_Fragments.size() - 1].m_Material = material;
		}
		else { // Add a new fragment

			ObjectFragment fragment = {
				pos,
				vel,
				material
			};
			m_Fragments.push_back(fragment);
		}
	}

	Object::ObjectFragment Object::GetActiveFragment(float time) {

		LF_CORE_ASSERT(time >= m_TimeOfCreation, "Cannot find active fragment before object was created!");

		uint32_t currentFragmentIndex = 0;
		for (uint32_t i = 0; i < m_Fragments.size(); i++) {
			float fragmentTime = m_Fragments[i].m_StartPos.w;
			if (time >= fragmentTime)
				currentFragmentIndex = i;
		}

		ObjectFragment fragment = m_Fragments[currentFragmentIndex];
		return fragment;
	}

	glm::vec4 Object::Get4Pos(float time) {

		ObjectFragment fragment = GetActiveFragment(time);
		return fragment.m_StartPos + (time - fragment.m_StartPos.w) * glm::vec4(fragment.m_StartVel, 1.0f);
	}
}