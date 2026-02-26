// Shaders/mesh.hlsl

cbuffer CBPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4   gLightPosWS;           // xyz = position, w unused
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
    float3 posWS    : TEXCOORD0; // World space position
    float3 normalWS : TEXCOORD1; // World space normal
    float3 color    : COLOR;
};

VSOut VS(VSIn i)
{
    VSOut o;

    float4 worldPos = mul(float4(i.pos, 1.0f), gWorld);
    o.posWS = worldPos.xyz;
    o.pos = mul(float4(i.pos, 1.0f), gWorldViewProj);

    // 法线：用 world inverse transpose（w=0 表示方向）
    float3 n = mul(float4(i.normal, 0.0f), gWorldInvTranspose).xyz;
    o.normalWS = n;

    o.color = i.color.rgb;
    return o;
}

float4 PS(VSOut i) : SV_TARGET
{
    // Point light (position from CPU)
    float3 lightPos = gLightPosWS.xyz;
    
    // Calculate light direction from surface to light
    float3 toLightVec = lightPos - i.posWS;
    float distance = length(toLightVec);
    float3 L = toLightVec / distance;  // normalize
    
    // Lambert diffuse
    float3 N = normalize(i.normalWS);
    float ndl = max(0.0, dot(N, L));
    
    // Distance attenuation (inverse square with minimum)
    float attenuation = 1.0 / max(1.0, distance * distance * 0.1);
    
    // Lighting calculation
    float3 base = i.color;
    float ambient = gLightColorAndAmbient.w;
    float3 lightColor = gLightColorAndAmbient.xyz;
    float3 diffuse = base * ndl * lightColor * attenuation;
    float3 ambientColor = base * ambient;
    float3 lit = ambientColor + diffuse;

    return float4(lit, 1.0f);
}
