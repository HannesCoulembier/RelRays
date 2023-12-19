#include "lfpch.h"
#include "RelRays/Core/Model.h"

namespace RelRays {

	Model::Model(const std::string& objPath) {

		m_Vertices = {
			{ { -1.0f, -1.0f, 0.0f } },
			{ { -1.0f, 1.0f, 0.0f } },
			{ { 1.0f, -1.0f, 0.0f } },
			{ { 1.0f, 1.0f, 0.0f } },
		};
		m_Indices = {
			0, 1, 2, 1, 2, 3
		};
	}
}