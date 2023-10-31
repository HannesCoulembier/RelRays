#pragma once
#include "LoFox/Renderer/IndexBuffer.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Buffer.h"

namespace LoFox {

	struct VulkanIndexBufferData {

		Ref<Buffer> Buffer;
	};

	class VulkanIndexBuffer : public IndexBuffer {

	public:
		VulkanIndexBuffer(uint32_t numberOfIndices, const uint32_t* data);

		virtual void Destroy() override;
	private:
		VulkanIndexBufferData m_VulkanData;
	};
}