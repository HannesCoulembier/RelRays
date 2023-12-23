#include "lfpch.h"
#include "LoFox/Renderer/Resources/StorageBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/Resources/OpenGLStorageBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/Resources/VulkanStorageBuffer.h"
#endif

namespace LoFox {

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t maxObjectCount, uint32_t objectSize) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLStorageBuffer>(maxObjectCount, objectSize);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanStorageBuffer>(maxObjectCount, objectSize);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	StorageBuffer::StorageBuffer(uint32_t maxObjectCount, uint32_t objectSize)
		: m_MaxObjectCount(maxObjectCount), m_ObjectSize(objectSize), m_Size(maxObjectCount*objectSize) {}
}