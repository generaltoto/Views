#include "VECS_RenderSystem.h"

#include "Engine/Engine.h"
#include "D3D/Base/D3DRenderer.h"

void VECS_RenderSystem::Awake()
{
}

void VECS_RenderSystem::Start()
{
}

void VECS_RenderSystem::Update(float delta)
{
}

void VECS_RenderSystem::Render()
{
    // Each entity's renderer needs to be rendered, but each renderer as its own way to be rendered
    for (auto& entity : m_RegisteredEntities)
    {
        ID3D12GraphicsCommandList* commandList = I(D3DRenderer)->GetCommandList();

        // The render order is important, we do not want to render a mesh that is behind a UI element.
        // This saves us a lot of pixel rewrite

        // First, we render our UI elements that might cover a lot of objects
        if (UIRenderer* uiRenderer = Coordinator::GetInstance()->GetComponent<UIRenderer>(entity))
        {
            uiRenderer->Render(commandList);
        }
        
        if (MeshRenderer* meshRenderer = Coordinator::GetInstance()->GetComponent<MeshRenderer>(entity))
        {
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
                if (viewFrustum.Contains(bs) != DirectX::DISJOINT)
                    meshRenderer->Render(commandList);
            }
            else
            {
                // If the mesh is not clippable, we render it anyway
                meshRenderer->Render(commandList);
            }
        }

        // Render the particles after the meshRenderers to avoid depth buffer issues (particles are transparent)
        if (ParticleRenderer* particleRenderer = Coordinator::GetInstance()->GetComponent<ParticleRenderer>(entity))
        {
            particleRenderer->Render(commandList);
        }

        // We render the skybox at the end as it is the most likely to be hidden by other objects
        if (SkyRenderer* skyRenderer = Coordinator::GetInstance()->GetComponent<SkyRenderer>(entity))
        {
            skyRenderer->Render(commandList);
        }

        NULLPTR(commandList)
    }
}
