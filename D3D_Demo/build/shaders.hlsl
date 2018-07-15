cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float4x4 mWorldViewProj;
}


struct VIn
{
	float3 position : POSITION;
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
	//output.position = float4(input.position, 1.0f);
	output.color = float4(input.position,0.0f);
	
	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;

}