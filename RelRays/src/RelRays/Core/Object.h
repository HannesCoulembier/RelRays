#pragma once

#include <LoFox.h>

#include "RelRays/Core/Material.h"
#include "RelRays/Core/Model.h"

namespace RelRays {

	class Environment; // Forward declaration

	class Object {

	public:
		Object(LoFox::Ref<Environment> origin, glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model);

		void SetPos(glm::vec3 pos);
		void SetVel(glm::vec3 vel);
		void SetState(glm::vec3 pos, glm::vec3 vel);
		void AddEvent(glm::vec4 pos, glm::vec3 vel, LoFox::Ref<Material> material);

		glm::vec4 Get4Pos(float time); // Get the 4 position vector relative to the origin (time is also origin time)

	private:
		class ObjectFragment {

			ObjectFragment(glm::vec4 pos, glm::vec3 vel, LoFox::Ref<Material> material);
			glm::vec4 m_StartPos = glm::vec4(0.0f);
			glm::vec3 m_StartVel = glm::vec3(0.0f);
			LoFox::Ref<Material> m_Material = nullptr;

			friend Object;
			friend Environment;
		};
	private:
		inline ObjectFragment GetLatestFragment() { return m_Fragments[m_Fragments.size() - 1]; }
		ObjectFragment GetActiveFragment(float time);
	private:

		LoFox::Ref<Environment> m_Origin;
		float m_ProperTime = 0.0f;
		float m_TimeOfCreation; // The time of creation relative to the origin

		LoFox::Ref<Model> m_Model;
		std::vector<ObjectFragment> m_Fragments;

		friend Environment;
	};
}