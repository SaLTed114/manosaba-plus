Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

cbuffer CBFrame : register(b0)
{
    float4x4 gViewProj;
};

cbuffer CBCard : register(b1)
{
    float3 gPos;
    float  gYaw;
    float2 gSize;        // (w,h)
    float  gAlphaCut;
    float  _pad;
};

struct VSIn {
    float2 pos : POSITION; // local quad (x,y)
    float2 uv  : TEXCOORD0;
};

struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOut VS(VSIn i)
{
    VSOut o;
    // local: x in [-0.5,0.5]*w, y in [0,1]*h (脚底在 pos)
    float3 p;
    p.x = i.pos.x * gSize.x;
    p.y = i.pos.y * gSize.y;
    p.z = 0.0;

    float c = cos(gYaw), s = sin(gYaw);
    float3 pr;
    pr.x =  c*p.x + s*p.z;
    pr.z = -s*p.x + c*p.z;
    pr.y =  p.y;

    float3 worldPos = pr + gPos;
    o.pos = mul(float4(worldPos, 1.0), gViewProj);
    o.uv = i.uv;
    return o;
}

float4 PS(VSOut i, bool frontFace : SV_IsFrontFace) : SV_TARGET
{
    float4 tex = gTex.Sample(gSamp, i.uv);
    clip(tex.a - gAlphaCut);

    if (!frontFace) {
        return float4(0.0, 0.0, 0.0, tex.a); // 背面黑剪影，保留alpha轮廓
    }
    return tex;
}
