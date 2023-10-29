#pragma once

namespace LoFox {

	class StorageBuffer {

	public:
		virtual void Destroy() = 0;

		virtual void SetData(uint32_t objectCount, const void* data) = 0; // Sets the first objectCount objects in buffer to data
		void* GetData() { return m_Data; }

		static Ref<StorageBuffer> Create(uint32_t maxObjectCount, uint32_t objectSize);
	protected:
		StorageBuffer(uint32_t maxObjectCount, uint32_t objectSize);
	protected:
		void* m_Data = nullptr;
		uint32_t m_MaxObjectCount = 0;
		uint32_t m_ObjectSize = 0;
		uint32_t m_Size = 0;
	};
}




// #pragma once
// 
// #include "LoFox/Core/Core.h"
// 
// #include <vulkan/vulkan.h>
// 
// #include "LoFox/Renderer/Buffer.h"
// 
// namespace LoFox {
// 
// 	class StorageBuffer {
// 
// 	public:
// 		StorageBuffer(uint32_t objectCount, uint32_t objectSize);
// 		void Destroy();
// 
// 		void SetData(const void* data, uint32_t count);
// 
// 		std::vector<VkDescriptorBufferInfo> GetDescriptorInfos();
// 
// 		static Ref<StorageBuffer> Create(uint32_t bufferSize, uint32_t objectSize);
// 	private:
// 		uint32_t m_Size, m_ObjectCount, m_ObjectSize;
// 
// 		std::vector<Ref<Buffer>> m_Buffers;
// 		std::vector<void*> m_BuffersMapped;
// 	};
// }