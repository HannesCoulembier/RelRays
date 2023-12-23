#include "lfpch.h"
#include "LoFox/Renderer/VertexBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanVertexBuffer.h"
#endif

namespace LoFox {

	uint32_t GetVertexAttributeTypeSize(VertexAttributeType type) {
	
		switch (type) {
			case VertexAttributeType::Float:	return 4 * 1;
			case VertexAttributeType::Float2:	return 4 * 2;
			case VertexAttributeType::Float3:	return 4 * 3;
			case VertexAttributeType::Float4:	return 4 * 4;

			case VertexAttributeType::Mat3:		return 4 * 3 * 3;
			case VertexAttributeType::Mat4:		return 4 * 4 * 4;

			case VertexAttributeType::Int:		return 4 * 1;
			case VertexAttributeType::Int2:		return 4 * 2;
			case VertexAttributeType::Int3:		return 4 * 3;
			case VertexAttributeType::Int4:		return 4 * 4;

			case VertexAttributeType::Uint:		return 4 * 1;
			case VertexAttributeType::Bool:		return 1;
		}
	
		LF_CORE_ASSERT(false, "Unknown vertex attribute type!");
		return 0;
	}

	uint32_t GetVertexAttributeComponentCount(VertexAttributeType attribute) {

		switch (attribute) {

			case VertexAttributeType::Float:	return 1;
			case VertexAttributeType::Float2:	return 2;
			case VertexAttributeType::Float3:	return 3;
			case VertexAttributeType::Float4:	return 4;

			case VertexAttributeType::Mat3:		return 3; // 3 * float3
			case VertexAttributeType::Mat4:		return 4; // 4 * float4

			case VertexAttributeType::Int:		return 1;
			case VertexAttributeType::Int2:		return 2;
			case VertexAttributeType::Int3:		return 3;
			case VertexAttributeType::Int4:		return 4;

			case VertexAttributeType::Uint:		return 1;
			case VertexAttributeType::Bool:		return 1;
		}

		LF_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexLayout::VertexLayout(std::initializer_list<VertexAttributeType> attributes)
		: m_Attributes(attributes) {

		VertexSize = 0;
		for (auto attribute : m_Attributes)
			VertexSize += GetVertexAttributeTypeSize(attribute);
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t bufferSize, const void* data, VertexLayout layout) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLVertexBuffer>(bufferSize, data, layout);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanVertexBuffer>(bufferSize, data, layout);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	VertexBuffer::VertexBuffer(uint32_t bufferSize, VertexLayout layout)
		: m_VertexLayout(layout), m_Size(bufferSize) {}
}