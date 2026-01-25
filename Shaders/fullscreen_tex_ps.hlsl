Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VSOut i) : SV_TARGET {
    return gTex.Sample(gSamp, i.uv);
}
