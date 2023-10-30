#pragma once
#include "LoFox/Renderer/Resource.h"

#include <glad/glad.h>

namespace LoFox {

	void BindResourceLayout(GLenum program, Ref<ResourceLayout> layout);

	struct OpenGLResourceLayoutData {

	};

	class OpenGLResourceLayout : public ResourceLayout {

	public:
		OpenGLResourceLayout(std::initializer_list<Resource> resources);

		virtual void Destroy() override;
	private:
		OpenGLResourceLayoutData m_OpenGLData = {};
	};
}
