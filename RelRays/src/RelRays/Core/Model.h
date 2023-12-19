#pragma once

namespace RelRays {

	class Environment; // Forward declaration

	struct ModelVertex {

		glm::vec3 Position;
	};

	class Model {
		
	public:
		Model(const std::string& objPath);
	private:
		std::vector<ModelVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		friend Environment;
	};
}