#include "VECS_UiRenderSystem.h"

#include "D3D/Base/VGHandler.h"

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
        VGUiRenderer* uiRenderer = I(Coordinator)->GetComponent<VGUiRenderer>(entity);
        uiRenderer->Update(delta);
    }
}

void VECS_UiRenderSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(VGHandler)->GetCommandList();
    for (const auto& entity : RegisteredEntities)
    {
        VGUiRenderer* uiRenderer = I(Coordinator)->GetComponent<VGUiRenderer>(entity);
        uiRenderer->Render(commandList);
    }
}

