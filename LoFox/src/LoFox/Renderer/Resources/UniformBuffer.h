#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	class UniformBuffer {

	public:
		UniformBuffer(uint32_t bufferSize);
		void Destroy();

		void SetData(const void* data);

		std::vector<VkDescriptorBufferInfo> GetDescriptorInfos();

		static Ref<UniformBuffer> Create(uint32_t bufferSize);
	private:
		uint32_t m_Size;

		std::vector<Ref<Buffer>> m_Buffers;
		std::vector<void*> m_BuffersMapped;
	};
}