#include "lfpch.h"
#include "Platform/OpenGL/OpenGLResource.h"

#include <glad/glad.h>

#include "Platform/OpenGL/Resources/OpenGLUniformBuffer.h"
#include "Platform/OpenGL/Resources/OpenGLStorageBuffer.h"

namespace LoFox {

	void BindResourceLayout(Ref<ResourceLayout> layout) {

		std::vector<Resource> resources = layout->GetResources();
		uint32_t binding = 0;
		for (auto resource : resources) {

			switch (resource.Type) {
				case ResourceType::UniformBufferResource: glBindBufferBase(GL_UNIFORM_BUFFER, binding, static_cast<OpenGLUniformBufferData*>(resource.UniformBufferRef->GetData())->RendererID); break;
				case ResourceType::StorageBufferResource: glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, static_cast<OpenGLStorageBufferData*>(resource.StorageBufferRef->GetData())->RendererID); break;
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