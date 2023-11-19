#include "World.h"

namespace RelRays {

	void World::Init() {

	}

	LoFox::Ref<World> World::Create() {

		LoFox::Ref<World> world = LoFox::CreateRef<World>();
		world->SetSelf(world);
		world->Init();
		return world;
	}

	void World::SetSelf(LoFox::Ref<World> world) {

		m_Self = world;
	}

	void World::OnUpdate(float ts) {

	}

	void World::RenderFrame() {

		LoFox::Renderer::BeginFrame({ 1.0f, 0.0f, 1.0f });

		// m_ComputePipeline->Dispatch(m_RickTexture->GetWidth(), m_RickTexture->GetHeight(), 8, 8);

		// LoFox::Renderer::SetActivePipeline(m_GraphicsPipeline);
		// LoFox::Renderer::Draw(m_IndexBuffer, m_VertexBuffer);

		LoFox::Renderer::EndFrame();
	}

	void World::Destroy() {

		LF_INFO("Deleting the world!");
	}

	LoFox::Ref<Object> World::CreateObject() {

		LoFox::Ref<Object> object = LoFox::CreateRef<Object>(m_Self);
		m_Objects.push_back(object);
		return object;
	}
}