#include "lfpch.h"
#include "Platform/OpenGL/OpenGLResource.h"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLContext.h"

#include "Platform/OpenGL/Resources/OpenGLUniformBuffer.h"
#include "Platform/OpenGL/Resources/OpenGLStorageBuffer.h"
#include "Platform/OpenGL/Resources/OpenGLTexture.h"
#include "Platform/OpenGL/Resources/OpenGLStorageImage.h"

namespace LoFox {

	void BindResourceLayout(GLenum program, Ref<ResourceLayout> layout) {

		std::vector<Resource> resources = layout->GetResources();
		uint32_t binding = 0;
		for (auto resource : resources) {

			switch (resource.Type) {
				case ResourceType::UniformBufferResource: {

					glBindBufferBase(GL_UNIFORM_BUFFER, binding, static_cast<OpenGLUniformBufferData*>(resource.UniformBufferRef->GetData())->RendererID); break;
				}
				case ResourceType::StorageBufferResource: {

					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, static_cast<OpenGLStorageBufferData*>(resource.StorageBufferRef->GetData())->RendererID); break;
				}
				case ResourceType::TextureResource: {

					glActiveTexture(GL_TEXTURE0 + binding);
					glBindTexture(GL_TEXTURE_2D, static_cast<OpenGLTextureData*>(resource.TextureRef->GetData())->RendererID);
					break;
				}
				case ResourceType::StorageImageResource: {

					glBindImageTexture(binding, static_cast<OpenGLStorageImageData*>(resource.StorageImageRef->GetData())->RendererID, 0, 0, 0, GL_READ_WRITE, GL_RGBA8);
				}
				case ResourceType::StorageImageAsTextureResource: {

					glActiveTexture(GL_TEXTURE0 + binding);
					glBindTexture(GL_TEXTURE_2D, static_cast<OpenGLStorageImageData*>(resource.StorageImageRef->GetData())->RendererID);
					break; 
				}
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