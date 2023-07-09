#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"

namespace LoFox {

	class RenderCommand {

	public:
		static void Init();
		static void Shutdown();
		
		static void SetClearColor(glm::vec4 backGroundColor);
		static void SetViewport(glm::vec2 offset, glm::vec2 size);
		static void SetScissor(glm::vec2 offset, glm::vec2 size);
		static void SubmitVertexBuffer(Ref<VertexBuffer> buffer);
		static void SubmitIndexBuffer(Ref<IndexBuffer> buffer);

		static inline std::vector<VkClearValue> GetClearValues() { return m_ClearValues; }
		static inline std::vector<VkBuffer> GetVertexBuffers() { return m_VertexBuffers; }
		static inline std::vector<VkDeviceSize> GetVertexBufferOffsets() { return m_VertexBufferOffsets; }
		static inline VkBuffer GetIndexBuffer() { return m_IndexBuffer; }
		static inline uint32_t GetNumberOfIndices() { return m_NumberOfIndices; }
		static inline VkViewport GetViewport() { return m_Viewport; }
		static inline VkRect2D GetScissor() { return m_Scissor; }
	private:
		static std::vector<VkClearValue> m_ClearValues;

		static uint32_t m_VertexBuffersTotalOffset;
		static std::vector<VkDeviceSize> m_VertexBufferOffsets;
		static std::vector<VkBuffer> m_VertexBuffers;

		static VkBuffer m_IndexBuffer;
		static uint32_t m_NumberOfIndices;

		static VkViewport m_Viewport;
		static VkRect2D m_Scissor;
	};
}