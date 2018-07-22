cbuffer mtw : register( b0 )
{
    matrix modelToWorldMatrix;
};

cbuffer wtv : register( b1 )
{
    matrix worldtoViewMatrix;
};

cbuffer proj : register( b2 )
{
    matrix projectionMatrix;
};

cbuffer LightColor
{
	float3 lightColor;
};

cbuffer LightPos
{
	float3 lightPos_WorldSpace;
};

cbuffer DiffuseColor
{
	float3 diffuseLightColor;
};

cbuffer DiffuseIntensity
{
	float3 diffuseLightIntensity;
};

cbuffer AmbientColor
{
	float3 ambientLightColor;
};

cbuffer AmbientIntensity
{
	float3 ambientLightIntensity;
};

cbuffer SpecularColor
{
	float3 specularLightColor;
};

cbuffer SpecularIntensity
{
	float3 specularLightIntensity;
};

cbuffer SpecularPower
{
	float specularPower;
};

cbuffer CameraPosition
{
	float3 cameraPosition_WorldSpace;
};

cbuffer ITMTW
{
	float4x4 inverseTransposeModelToWorldMatrix;
};

cbuffer Interp
{
	float interp;
};

struct VIn
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VOut
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 fragNormWorld : TEXCOORD1;
	float3 fragPosWorld : TEXCOORD2;
};

VOut VertexShaderFunction(VIn input)
{
    VOut output = (VOut)0;

    output.position = input.position;
	output.position = mul( output.position, modelToWorldMatrix);
	output.fragPosWorld = (float3)output.position;
	
    output.position = mul( output.position, worldtoViewMatrix);
    output.position = mul( output.position, projectionMatrix);
	output.fragNormWorld = (float3)mul(input.normal, inverseTransposeModelToWorldMatrix);
	output.color = (float3)input.color;
	output.uv = (float2)input.uv;
	
    return output;
}


float4 PixelShaderFunction(VOut input) : SV_TARGET
{
	float3 positionEye = normalize(lightPos_WorldSpace - input.fragPosWorld);
	float3 normalNormEye = normalize(input.fragNormWorld);
	
	float3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);
	float3 ambientLight = ambientLightColor * ambientLightIntensity;
		
	float3 viewDirection = normalize(cameraPosition_WorldSpace - input.fragPosWorld);
	float3 reflectionDirection = reflect(normalize(input.fragPosWorld - lightPos_WorldSpace), normalNormEye);
	float3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);
	float3 totalLight = ambientLight + diffuseLight + specularLight;
	
	float4 color;
	color.rgb = clamp((totalLight)* lightColor, 0.0f, 1.0f) * interp + (1.0f - interp) * input.color;
	color.a = 1.0f;
	return color;
}