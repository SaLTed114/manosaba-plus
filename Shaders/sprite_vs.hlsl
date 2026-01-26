cbuffer CBFrame : register(b0)
{
    float2 gScreenSize;  // (W,H)
    float2 _pad;
};

struct VSIn {
    float2 posPx : POSITION;   // 像素坐标（左上为原点，y向下）
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

struct VSOut {
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

VSOut VS(VSIn i) {
    VSOut o;

    float2 ndc;
    ndc.x = (i.posPx.x / gScreenSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (i.posPx.y / gScreenSize.y) * 2.0;

    o.pos = float4(ndc, 0.0, 1.0);
    o.uv = i.uv;
    o.color = i.color;
    return o;
}
