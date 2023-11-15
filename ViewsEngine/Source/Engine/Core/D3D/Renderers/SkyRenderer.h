#pragma once 

/*
This class is responsible for rendering the skybox.
It is not working yet.
*/
class SkyRenderer final : public Component, public IRenderer
{
	friend class VECS_SkyRendererSystem;

public:
	SkyRenderer();
	~SkyRenderer() override;

private:
	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;
};