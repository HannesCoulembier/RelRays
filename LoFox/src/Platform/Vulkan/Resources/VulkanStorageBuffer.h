#pragma once

#include "LoFox/Renderer/Resources/StorageBuffer.h"

#include "Platform/Vulkan/Buffer.h"

namespace LoFox {

	struct VulkanStorageBufferData {

		VkDescriptorBufferInfo BufferInfo;
	};

	class VulkanStorageBuffer : public StorageBuffer {

	public:
		VulkanStorageBuffer(uint32_t maxObjectCount, uint32_t objectSize);
		virtual void Destroy() override;

		virtual void SetData(uint32_t objectCount, const void* data) override;
	private:
		VulkanStorageBufferData m_VulkanData;

		Ref<Buffer> m_Buffer;
		void* m_MappedBuffer;
	};
}
