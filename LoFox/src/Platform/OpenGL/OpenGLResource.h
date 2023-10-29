#pragma once

#include "LoFox/Renderer/Resource.h"

namespace LoFox {

	void BindResourceLayout(Ref<ResourceLayout> layout);

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
