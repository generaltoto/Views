#pragma once

class VECS_RenderSystem : public System
{
    // Allow D3DRenderer to access the private Render() method
    // There is no better way to do this right now
    // TODO: Find a better way to do this
    friend class D3DRenderer;
    
public:
    void Awake() override;
    void Start() override;
    void Update(float delta) override;

private:
    void Render();
};
