////////////////////////////////////////////////////////////////////////////


cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
	uint objectID : packoffset(c4.x);
	uint gnMaterial : packoffset(c4.y);
};
Texture2DArray gtxtTextureArray : register(t0);
SamplerState gssDefaultSamplerState : register(s0);

#include "Light.hlsl"


/////////////////////////////////////////////////////////////////////////////

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.uv = input.uv;
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);
	//color = float4(input.positionW, 1.0f);
	return(color);
}

///////////////////////////////////////////////////////////////////////////////////////////

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 Normal : SV_TARGET0;
	float4 Position : SV_TARGET1;
	float4 Texture : SV_TARGET2;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSObject(VS_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;

	output.Position = float4(input.positionW,1.0f);
	input.normalW = normalize(input.normalW);
	output.Normal = float4(input.normalW.xyz, (float)objectID);
	//float3 uvw = float3(input.uv, nPrimitiveID / 2);
	//output.Texture = gtxtTextureArray.Sample(gssDefaultSamplerState, uvw);
	output.Texture = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return(output);
}

///////////////////////////////////////////////////////////////////////////////////////////
//정점 셰이더의 입력을 위한 구조체를 선언한다.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
//정점 셰이더를 정의한다.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView),
		gmtxProjection);
	output.color = input.color;
	return(output);
}
//픽셀 셰이더를 정의한다.
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//
//struct VS_TEXTURED_LIGHTING_INPUT
//{
//	float3 position : POSITION;
//	float3 normal : NORMAL;
//	float2 uv : TEXCOORD;
//};
//struct VS_TEXTURED_LIGHTING_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float3 positionW : POSITION;
//	float3 normalW : NORMAL0;
//	float3 normalV : NORMAL1;
//	float2 uv : TEXCOORD;
//};
//
//struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
//{
//	float4 Position : SV_TARGET0;
//	float4 Normal : SV_TARGET1;
//	float4 Texture : SV_TARGET2;
//	float2 ObjectIDzDepth : SV_TARGET3;
//};
//
//VS_TEXTURED_LIGHTING_OUTPUT VSObject(VS_TEXTURED_LIGHTING_INPUT input)
//{
//	VS_TEXTURED_LIGHTING_OUTPUT output;
//
//	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
//	output.normalV = mul(float4(output.normalW, 1.0f), gmtxView).xyz;
//	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
//	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//	output.uv = input.uv;
//
//	return(output);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//struct VS_SCREEN_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD0;
//	float3 viewSpaceDir : TEXCOORD1;
//};
//
//VS_SCREEN_OUTPUT VSScreen(uint nVertexID : SV_VertexID)
//{
//	VS_SCREEN_RECT_TEXTURED_OUTPUT output = (VS_SCREEN_RECT_TEXTURED_OUTPUT)0;
//
//	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
//	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
//	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
//
//	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
//	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
//	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }
//
//	output.viewSpaceDir = mul(output.position, gmtxInverseProjection).xyz;
//
//	return(output);
//}
//
//static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
//static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };
//
//float4 Edge(float4 position)
//{
//	float3 EdgeColor = float3(0, 0, 1);
//	int EdgeSize = 1;
//
//	int Edge = false;
//	float fObjectID = gtxtInputTextures[4][int2(position.xy)].r;
//
//	for (int i = 0; i < EdgeSize; i++)
//	{
//		if (fObjectID != gtxtInputTextures[4][int2(position.xy) + gnOffsets[i]].r) Edge = true; // 오브젝트 별 테두리
//	}
//
//	if (Edge)
//		return(float4(LineColor, 1));
//	else
//		return(float4(0, 0, 0, 0));
//}
//
//Texture2D gtxtInputTextures[5] : register(t1); //Position, Normal, Texture, ObjectID + zDepth, Depth
//
//float4 PSScreen(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
//{
//	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
//	cColor = gtxtInputTextures[2].Sample(gssDefaultSamplerState, input.uv) *
//				Lighting(gtxtInputTextures[0].Sample(gssDefaultSamplerState, input.uv) ,gtxtInputTextures[1].Sample(gssDefaultSamplerState, input.uv)) +
//				LaplacianEdge(input.position);
//	return(cColor);
//}