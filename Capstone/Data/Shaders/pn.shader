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
    float3 normal : NORMAL0;
};

VOut VertexShaderFunction(float4 position : POSITION, float3 normal : NORMAL0)
{
    VOut output;

    output.position = position;
	output.position = mul( output.position, modelToWorldMatrix);
    output.position = mul( output.position, worldtoViewMatrix);
    output.position = mul( output.position, projectionMatrix);
    output.normal = normal;

    return output;
}


float4 PixelShaderFunction(VOut input) : SV_TARGET
{
	float4 color;
	color.rgb = input.normal;
	color.a = 1.0;
    return color;
}