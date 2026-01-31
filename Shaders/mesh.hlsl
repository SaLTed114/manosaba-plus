// Shaders/mesh.hlsl

cbuffer CBPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4   gLightDirWS;           // xyz = direction, w unused
    float4   gLightColorAndAmbient; // xyz = color, w = ambient
};

struct VSIn
{
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD0;
    float4 color  : COLOR;  // Changed to float4 to match input layout
};

struct VSOut
{
    float4 pos      : SV_POSITION;
    float3 normalWS : TEXCOORD0; // 借用一个 TEXCOORD 传法线
    float3 color    : COLOR;
};

VSOut VS(VSIn i)
{
    VSOut o;

    float4 worldPos = mul(float4(i.pos, 1.0f), gWorld);
    o.pos = mul(float4(i.pos, 1.0f), gWorldViewProj);

    // 法线：用 world inverse transpose（w=0 表示方向）
    float3 n = mul(float4(i.normal, 0.0f), gWorldInvTranspose).xyz;
    o.normalWS = n;

    o.color = i.color.rgb;
    return o;
}

float4 PS(VSOut i) : SV_TARGET
{
    float3 N = normalize(i.normalWS);
    float3 L = normalize(-gLightDirWS.xyz);
    float ndl = max(0.0, dot(N, L));

    float3 base = i.color;
    float ambient = gLightColorAndAmbient.w;
    float3 lightColor = gLightColorAndAmbient.xyz;
    float3 lit = base * (ambient + ndl * (1.0 - ambient)) * lightColor;

    return float4(lit, 1.0f);
}
