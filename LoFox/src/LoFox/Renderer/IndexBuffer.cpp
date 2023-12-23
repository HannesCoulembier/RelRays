#include "lfpch.h"
#include "IndexBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanIndexBuffer.h"
#endif

namespace LoFox {

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t numberOfIndices, const uint32_t* data) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLIndexBuffer>(numberOfIndices, data);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanIndexBuffer>(numberOfIndices, data);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	IndexBuffer::IndexBuffer(uint32_t numberOfIndices, const uint32_t* data)
		: m_NumberOfIndices(numberOfIndices), m_Size(m_NumberOfIndices * m_IndexSize) {}
}