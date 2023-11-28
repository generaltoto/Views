#pragma once

struct VGPassConstant
{
    VGPassConstant() = default;
    
    DirectX::XMFLOAT4X4 View = Identity4X4();
    DirectX::XMFLOAT4X4 OrthoProj = Identity4X4();
    DirectX::XMFLOAT4X4 ViewProj = Identity4X4();

    DirectX::XMFLOAT4 LightColor = {1.0f, 0.0f, 1.0f, 1.0f};

    DirectX::XMFLOAT3 EyePosW = {0.0f, 0.0f, 0.0f};
    float TotalTime = 0.0f;

    DirectX::XMFLOAT3 LightDirection = {-1.0f, -1.0f, 0.0f};
    float DeltaTime = 0.0f;
};
