#pragma once 

/*
This class is responsible for rendering the skybox.
It is not working yet.
*/
class VGSkyRenderer final : public VGIRenderer
{
	friend class VECS_SkyRendererSystem;

public:
	VGSkyRenderer();
	~VGSkyRenderer() override;

private:
	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;
};