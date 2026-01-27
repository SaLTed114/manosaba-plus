// Shaders/mesh.hlsl

cbuffer CBPerFrame : register(b0)
{
    float4x4 gWorldViewProj;
};

struct VSIn
{
    float3 pos   : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
};

VSOut VS(VSIn i)
{
    VSOut o;
    o.pos = mul(float4(i.pos, 1.0f), gWorldViewProj);
    o.color = i.color;
    return o;
}

float4 PS(VSOut i) : SV_TARGET
{
    return i.color;
}
