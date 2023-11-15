#include "VECS_ParticleRendererSystem.h"

#include "D3D/Base/D3DRenderer.h"

void VECS_ParticleRendererSystem::Awake()
{
}

void VECS_ParticleRendererSystem::Start()
{
}

void VECS_ParticleRendererSystem::Update(const float delta)
{
    for (const auto& entity : RegisteredEntities)
    {
        ParticleRenderer* particleRenderer = I(Coordinator)->GetComponent<ParticleRenderer>(entity);
        particleRenderer->Update(delta);
    }
}

void VECS_ParticleRendererSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(D3DRenderer)->GetCommandList();
    
    for (const auto& entity : RegisteredEntities)
    {
        ParticleRenderer* particleRenderer = I(Coordinator)->GetComponent<ParticleRenderer>(entity);
        particleRenderer->Render(commandList);
    }
}

