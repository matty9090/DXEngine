cbuffer LightBuffer : register(b0) {
	float3 camPos;
	float3 lightPos;
	float3 lightCol;
	float3 ambientCol;

	float Pad0, Pad1, Pad2, Pad4;
};

struct PixelInputType {
	float4 Pos			: SV_POSITION;
	float3 WorldPos		: POSITION;
	float3 WorldNormal	: NORMAL;
	float4 Color		: COLOR;
};

float4 main(PixelInputType vOut) : SV_TARGET {
	float3 worldNormal	= normalize(vOut.WorldNormal);
	float3 CameraDir	= normalize(camPos - vOut.WorldPos.xyz);
	
	float  LightDist	= length(vOut.WorldPos - lightPos) / 340;
	float3 LightDir		= normalize(lightPos - vOut.WorldPos.xyz);
	float3 DiffuseLight	= lightCol * saturate(dot(worldNormal, LightDir));

	DiffuseLight = ambientCol + DiffuseLight;

	float4 colour;
	colour.rgb = DiffuseLight * vOut.Color;
	colour.a = 1.0f;

	return colour;
}
