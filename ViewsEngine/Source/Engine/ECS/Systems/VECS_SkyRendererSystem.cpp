#include "VECS_SkyRendererSystem.h"

#include "D3D/Base/D3DRenderer.h"

void VECS_SkyRendererSystem::Awake()
{
}

void VECS_SkyRendererSystem::Start()
{
}

void VECS_SkyRendererSystem::Update(float delta)
{
    // TODO : Update only the first sky renderer
    for (auto& entity : RegisteredEntities)
    {
        SkyRenderer* skyRenderer = I(Coordinator)->GetComponent<SkyRenderer>(entity);
        skyRenderer->Update(delta);
    }
}

void VECS_SkyRendererSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(D3DRenderer)->GetCommandList();
    
    for (auto& entity : RegisteredEntities)
    {
        SkyRenderer* skyRenderer = I(Coordinator)->GetComponent<SkyRenderer>(entity);
        skyRenderer->Render(commandList);
    }
}

