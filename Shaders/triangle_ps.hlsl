struct VSOut {
    float4 pos   : SV_POSITION;
    float3 color : COLOR;
};

float4 PS(VSOut pin) : SV_TARGET {
    return float4(pin.color, 1.0);
}
