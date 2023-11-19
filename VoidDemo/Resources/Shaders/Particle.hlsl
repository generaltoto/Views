struct Data
{
    float4x4 World;
    float4 Color1;
    float4 Color2;
    float AgeRatio;
};

StructuredBuffer<Data> gInstanceData : register(t0, space1);

cbuffer cbPass : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3 gEyePosW;
    float gTotalTime;
    float gDeltaTime;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT vs_main(VS_INPUT input, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;
    
    Data instance = gInstanceData[instanceID];
    float4x4 gWorld = instance.World;

    float4 posW = mul(float4(input.pos, 1.0f), gWorld);
    output.pos = mul(posW, gViewProj);
    
    float4 lerpColor = lerp(instance.Color1, instance.Color2, instance.AgeRatio);
    float4 color = float4(lerpColor.x, lerpColor.y, lerpColor.z, instance.AgeRatio);
    output.color = color;
    
    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    return input.color;
}