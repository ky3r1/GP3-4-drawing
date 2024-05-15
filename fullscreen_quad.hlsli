struct VS_IN
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 world_position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};