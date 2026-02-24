Texture2D gSceneCurr : register(t0);
Texture2D gScenePrev : register(t1);
SamplerState gSamp : register(s0);

cbuffer ComposeCB : register(b0) {
    float  gCrossfadeT;
    float3 pad;
};

struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOut VS(uint vid : SV_VertexID) {
    float2 pos[3] = {
        float2(-1.0, -1.0),
        float2(-1.0,  3.0),
        float2( 3.0, -1.0),
    };
    float2 uv[3] = {
        float2(0.0, 1.0),
        float2(0.0,-1.0),
        float2(2.0, 1.0),
    };

    VSOut o;
    o.pos = float4(pos[vid], 0.0, 1.0);
    o.uv  = uv[vid];
    return o;
}

float4 PS(VSOut i) : SV_TARGET {
    float t = saturate(gCrossfadeT);
    float4 colCurr = gSceneCurr.Sample(gSamp, i.uv);
    float4 colPrev = gScenePrev.Sample(gSamp, i.uv);
    return lerp(colPrev, colCurr, t);
}
