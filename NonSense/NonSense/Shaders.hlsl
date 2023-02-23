////////////////////////////////////////////////////////////////////////////


cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	matrix gmtxInverseProjection : packoffset(c8);
	float3 gf3CameraPosition : packoffset(c12);
	float3 gf3CameraDirection : packoffset(c13);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
	uint objectID : packoffset(c4.x);
	uint gnMaterial : packoffset(c4.y);
};

cbuffer cbDrawOptions : register(b5)
{
	float4 LineColor : packoffset(c0);
	uint DrawOption : packoffset(c1.x);
	uint LineSize : packoffset(c1.y);
	uint ToonShading : packoffset(c1.z);
};

Texture2DArray gtxtTextureArray : register(t0);
Texture2D gtxtInputTextures[4] : register(t1); //Position, Normal+ObjectID, Texture, Depth
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

VS_LIGHTING_OUTPUT VSObject(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.uv = input.uv;
	return(output);
}

///////////////////////////////////////////////////////////////////////////////////////////

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 Scene : SV_TARGET0;
	float4 Position : SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 Texture : SV_TARGET3;
};

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSObject(VS_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.Scene = float4(0, 1, 0, 0);

	output.Position = float4(input.positionW, 1.0f);
	output.Normal = float4(input.normalW.xyz, 1/((float)objectID+2));
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	output.Texture = gtxtTextureArray.Sample(gssDefaultSamplerState, uvw);

	return(output);
}

///////////////////////////////////////////////////////////////////////////////////////////
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;
	return(output);
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

///////////////////////////////////////////////////////////////////////////////////////


struct VS_SCREEN_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_SCREEN_OUTPUT VSScreen(uint nVertexID : SV_VertexID)
{
	VS_SCREEN_OUTPUT output = (VS_SCREEN_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }

	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };


float4 PSScreen(VS_SCREEN_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtInputTextures[2].Sample(gssDefaultSamplerState, input.uv);
	int Edge = false;
	float fObjectID = gtxtInputTextures[1][int2(input.position.xy)].a;

	for (int i = 0; i < LineSize; i++)
	{
		if(gtxtInputTextures[1][int2(input.position.xy) + gnOffsets[i]].a != 0 && fObjectID!=0)
		if (fObjectID != gtxtInputTextures[1][int2(input.position.xy) + gnOffsets[i]].a) Edge = true; // 오브젝트 별 테두리
	}

	if (Edge)
		return(float4(LineColor));
	else
		return(float4(cColor));
}

///////////////////////////////////////////////////////////////////////////////////////


struct VS_DEBUG_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_DEBUG_OUTPUT VSDebug(uint nVertexID : SV_VertexID)
{
	VS_SCREEN_OUTPUT output = (VS_SCREEN_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }

	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtInputTextures[2].Sample(gssDefaultSamplerState, input.uv);
	return(cColor);
}