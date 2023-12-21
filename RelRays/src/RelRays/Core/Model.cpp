#include "lfpch.h"
#include "RelRays/Core/Model.h"

#include "LoFox/Utils/Utils.h"

namespace RelRays {

	Model::Model(const std::string& objPath) {

		std::ifstream file(objPath);
		LF_CORE_ASSERT(file.is_open(), "Couldn't open obj file!");

		while (!file.eof()) {
			
			char cline[1024];
			file.getline(cline, 1024);
			std::string line = std::string(cline);
			std::vector<std::string> parts = LoFox::Utils::split(line, " ");

			if (parts.size() == 0)
				continue;

			if (parts[0] == "v") {

				ModelVertex mv = {};
				mv.Position = glm::vec3(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]));

				m_Vertices.push_back(mv);
			}
			if (parts[0] == "f") {

				m_Indices.push_back(std::stoi(parts[1]) - 1);
				m_Indices.push_back(std::stoi(parts[2]) - 1);
				m_Indices.push_back(std::stoi(parts[3]) - 1);
			}
		}

		// m_Vertices = {
		// 	{ { -1.0f, -1.0f, 0.0f } },
		// 	{ { -1.0f, 1.0f, 0.0f } },
		// 	{ { 1.0f, -1.0f, 0.0f } },
		// 	{ { 1.0f, 1.0f, 0.0f } },
		// };
		// m_Indices = {
		// 	0, 1, 2, 1, 2, 3
		// };
	}
}