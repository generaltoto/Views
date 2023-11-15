#include "VECS_UiRenderSystem.h"

#include "D3D/Base/D3DRenderer.h"

void VECS_UiRenderSystem::Awake()
{
}

void VECS_UiRenderSystem::Start()
{
}

void VECS_UiRenderSystem::Update(const float delta)
{
    for (const auto& entity : RegisteredEntities)
    {
        // Get the UI renderer bound to the entity
        UIRenderer* uiRenderer = I(Coordinator)->GetComponent<UIRenderer>(entity);
        uiRenderer->Update(delta);
    }
}

void VECS_UiRenderSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(D3DRenderer)->GetCommandList();
    for (const auto& entity : RegisteredEntities)
    {
        UIRenderer* uiRenderer = I(Coordinator)->GetComponent<UIRenderer>(entity);
        uiRenderer->Render(commandList);
    }
}

