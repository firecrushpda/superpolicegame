struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
	float4 inDiffuse : COLOR;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	sampleColor *= input.inDiffuse;
	return sampleColor;
}