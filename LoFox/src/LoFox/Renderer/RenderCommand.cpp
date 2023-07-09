#include "lfpch.h"
#include "LoFox/Renderer/RenderCommand.h"

namespace LoFox {

	std::vector<VkClearValue> RenderCommand::m_ClearValues;

	uint32_t RenderCommand::m_VertexBuffersTotalOffset;
	std::vector<VkDeviceSize> RenderCommand::m_VertexBufferOffsets;
	std::vector<VkBuffer> RenderCommand::m_VertexBuffers;

	VkBuffer RenderCommand::m_IndexBuffer;
	uint32_t RenderCommand::m_NumberOfIndices;

	VkViewport RenderCommand::m_Viewport;
	VkRect2D RenderCommand::m_Scissor;

	void RenderCommand::Init() {

		m_ClearValues.resize(2);
		m_VertexBuffersTotalOffset = 0;
		
		m_Viewport = {};
		m_Viewport.x = 0.0f;
		m_Viewport.y = 0.0f;
		m_Viewport.width = 0.0f;
		m_Viewport.height = 0.0f;
		m_Viewport.minDepth = 0.0f;
		m_Viewport.maxDepth = 1.0f;

		m_Scissor = {};
		m_Scissor.offset.x = 0.0f;
		m_Scissor.offset.y = 0.0f;
		m_Scissor.extent.width = 0.0f;
		m_Scissor.extent.height = 0.0f;
	}

	void RenderCommand::Shutdown() {

	}

	void RenderCommand::SubmitVertexBuffer(Ref<VertexBuffer> buffer) {

		m_VertexBuffers.push_back(buffer->GetBuffer());
		m_VertexBufferOffsets.push_back(m_VertexBuffersTotalOffset);
		m_VertexBuffersTotalOffset += buffer->GetSize();
	}

	void RenderCommand::SubmitIndexBuffer(Ref<IndexBuffer> buffer) {

		m_IndexBuffer = buffer->GetBuffer();
		m_NumberOfIndices = buffer->GetNumberOfIndices();
	}

	void RenderCommand::SetClearColor(glm::vec4 backGroundColor) {

		m_ClearValues[0] = { { backGroundColor.r, backGroundColor.g, backGroundColor.b, backGroundColor.a } };
		m_ClearValues[1] = { 1.0f, 0 };
	}

	void RenderCommand::SetViewport(glm::vec2 offset, glm::vec2 size) {

		m_Viewport.x = offset.x;
		m_Viewport.y = offset.y;
		m_Viewport.width = size.x;
		m_Viewport.height = size.y;
	}

	void RenderCommand::SetScissor(glm::vec2 offset, glm::vec2 size) {

		m_Scissor.offset.x = offset.x;
		m_Scissor.offset.y = offset.y;
		m_Scissor.extent.width = size.x;
		m_Scissor.extent.height = size.y;
	}
}