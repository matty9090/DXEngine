cbuffer MatrixBuffer : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType {
	float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float4 Color	: COLOR;
};

struct PixelInputType {
	float4 Pos			: SV_POSITION;
	float3 WorldPos		: POSITION;
	float3 WorldNormal	: NORMAL;
	float4 Color		: COLOR;
};

PixelInputType main(VertexInputType input) {
	PixelInputType output;

	float4 modelPos = float4(input.Pos, 1.0f);
	float4 worldPos = mul(modelPos, worldMatrix);
	output.WorldPos = worldPos.xyz;

	float4 modelNormal = float4(input.Normal, 0.0f);
	float4 worldNormal = normalize(mul(modelNormal, worldMatrix));
	output.WorldNormal = worldNormal.xyz;

	output.Pos = mul(worldPos, viewMatrix);
	output.Pos = mul(output.Pos, projectionMatrix);
	output.Color = input.Color;

	return output;
}