cbuffer cbChangesEveryFrame : register( b0 )
{
    matrix modelToWorldMatrix;
};

cbuffer cbChangesEveryFrame : register( b1 )
{
    matrix worldtoViewMatrix;
};

cbuffer cbChangeOnResize : register( b2 )
{
    matrix projectionMatrix;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VertexShaderFunction(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
	output.position = mul( output.position, modelToWorldMatrix);
    output.position = mul( output.position, worldtoViewMatrix);
    output.position = mul( output.position, projectionMatrix);
    output.color = color;

    return output;
}


float4 PixelShaderFunction(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}