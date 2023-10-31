#pragma once
#include "LoFox/Renderer/VertexBuffer.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Buffer.h"

namespace LoFox {

	namespace Utils {

		VkVertexInputBindingDescription VertexLayoutToVertexInputBindingDescription(uint32_t binding, const VertexLayout& layout);
		void AddVertexInputAttributeDescriptionsFromVertexLayout(uint32_t binding, const VertexLayout& layout, std::vector<VkVertexInputAttributeDescription>& attributes);
	}

	struct VulkanVertexBufferData {

		Ref<Buffer> Buffer;
	};

	class VulkanVertexBuffer : public VertexBuffer {

	public:
		VulkanVertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout);

		virtual void Destroy() override;
	private:
		VulkanVertexBufferData m_VulkanData;
	};
}