#pragma once
#include "LoFox/Renderer/Resources/UniformBuffer.h"

#include "Platform/Vulkan/Buffer.h"

namespace LoFox {

	struct VulkanUniformBufferData {

		VkDescriptorBufferInfo BufferInfo;
	};

	class VulkanUniformBuffer : public UniformBuffer {

	public:
		VulkanUniformBuffer(uint32_t size);
		virtual void Destroy() override;

		virtual void SetData(const void* data) override;
	private:
		VulkanUniformBufferData m_VulkanData;

		Ref<Buffer> m_Buffer;
		uint32_t m_Size;
		void* m_MappedBuffer;
	};
}
