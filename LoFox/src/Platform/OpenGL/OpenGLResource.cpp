#include "lfpch.h"
#include "Platform/OpenGL/OpenGLResource.h"

#include <glad/glad.h>

#include "Platform/OpenGL/Resources/OpenGLUniformBuffer.h"

namespace LoFox {

	void BindResourceLayout(Ref<ResourceLayout> layout) {

		std::vector<Resource> resources = layout->GetResources();
		uint32_t binding = 0;
		for (auto resource : resources) {

			switch (resource.Type) {
				case ResourceType::UniformBufferResource: glBindBufferBase(GL_UNIFORM_BUFFER, binding, static_cast<OpenGLUniformBufferData*>(resource.UniformBufferRef->GetData())->RendererID);
			}

			binding++;
		}
		
	}

	OpenGLResourceLayout::OpenGLResourceLayout(std::initializer_list<Resource> resources)
		: ResourceLayout(resources) {

		m_Data = &m_OpenGLData;
	}

	void OpenGLResourceLayout::Destroy() {

	}
}