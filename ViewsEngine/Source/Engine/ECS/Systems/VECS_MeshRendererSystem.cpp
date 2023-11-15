#include "VECS_MeshRendererSystem.h"

#include "Engine/Engine.h"
#include "D3D/Base/D3DRenderer.h"

void VECS_MeshRendererSystem::Awake()
{
}

void VECS_MeshRendererSystem::Start()
{
}

void VECS_MeshRendererSystem::Update(const float delta)
{
    for (const auto& entity : RegisteredEntities)
    {
        MeshRenderer* meshRenderer = I(Coordinator)->GetComponent<MeshRenderer>(entity);
        meshRenderer->Update(delta);
    }
}

void VECS_MeshRendererSystem::Render()
{
    ID3D12GraphicsCommandList* commandList = I(D3DRenderer)->GetCommandList();
    
    for (const auto& entity : RegisteredEntities)
    {
        MeshRenderer* meshRenderer = I(Coordinator)->GetComponent<MeshRenderer>(entity);

        BoundingFrustum frustum = Engine::GetMainCamera()->GetFrustum();

        const XMMATRIX view = Engine::GetMainCamera()->GetView();
        const XMMATRIX invView = XMMatrixInverse(nullptr, view);

        // Some mesh renderers are not clippable (e.g. spaceship parts)
        if (meshRenderer->IsClippable())
        {
            // Create a bounding sphere from the mesh renderer's transform
            // Note that we get the highest scale of the transform to make sure the bounding sphere is big enough
            // This could be improved by using the bounding box instead or multiplying the scale by the mesh radius
			
            BoundingSphere bs;
            bs.Center = meshRenderer->transform->GetPosition();
            bs.Radius = meshRenderer->transform->GetHighestScale();

            // Create a bounding frustum from the camera's view matrix
            BoundingFrustum viewFrustum;
            frustum.Transform(viewFrustum, invView);

            // If the bounding sphere is not in the camera's view, don't render the mesh
            if (viewFrustum.Contains(bs) == DirectX::DISJOINT) continue;
        }

        meshRenderer->Render(commandList);
    }
}

