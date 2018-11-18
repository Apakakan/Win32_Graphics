cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float4x4 mWorldViewProj;
}


struct VIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 uvCoord	: TEXCOORD;
};

struct VOut
{
	float4 position : SV_POSITION;	
	float4 color	: COLOR;
};

VOut VShader(VIn input)
{
	VOut output;

	output.position = mul(float4(input.position,1.0f), mWorldViewProj);
	output.color = float4(input.uvCoord,0.0f,0.0f);
	//output.color = float4(input.position, 1.0f);
	output.color = float4(input.normal, 1.0f);
	
	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}