Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOut VS(uint vid : SV_VertexID) {
    // 全屏三角形
    float2 pos[3] = { float2(-1,-1), float2(-1, 3), float2( 3,-1) };
    float2 uv [3] = { float2( 0, 1), float2( 0,-1), float2( 2, 1) };

    VSOut o;
    o.pos = float4(pos[vid], 0, 1);
    o.uv  = uv[vid];
    return o;
}
