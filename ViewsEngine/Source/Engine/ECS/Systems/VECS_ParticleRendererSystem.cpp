#include "VECS_ParticleRendererSystem.h"

#include "D3D/Base/VGHandler.h"

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
        VGParticleRenderer* particleRenderer = I(Coordinator)->GetComponent<VGParticleRenderer>(entity);
        particleRenderer->Update(delta);
    }
}

void VECS_ParticleRendererSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(VGHandler)->GetCommandList();
    
    for (const auto& entity : RegisteredEntities)
    {
        VGParticleRenderer* particleRenderer = I(Coordinator)->GetComponent<VGParticleRenderer>(entity);
        particleRenderer->Render(commandList);
    }
}

