struct VSIn {
    float3 pos   : POSITION;
    float3 color : COLOR;
};

struct VSOut {
    float4 pos   : SV_POSITION;
    float3 color : COLOR;
};

VSOut VS(VSIn vin) {
    VSOut o;
    o.pos = float4(vin.pos, 1.0);
    o.color = vin.color;
    return o;
}
