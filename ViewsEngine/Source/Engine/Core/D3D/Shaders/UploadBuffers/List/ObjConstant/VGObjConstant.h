#pragma once

struct ObjConstant
{
    ObjConstant() = default;
    DirectX::XMFLOAT4X4 World = Identity4X4();
};
