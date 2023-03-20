////////////////////////////////////////////////////////////////////////////
struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};


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
	uint objectID : packoffset(c4);
};

cbuffer cbMaterial : register(b3)
{
	MATERIAL gMaterial : packoffset(c0);
	uint gnTexturesMask : packoffset(c4);
}

cbuffer cbDrawOptions : register(b5)
{
	float4 LineColor : packoffset(c0);
	uint DrawOption : packoffset(c1.x);
	uint LineSize : packoffset(c1.y);
	uint ToonShading : packoffset(c1.z);
};

Texture2DArray gtxtTextureArray : register(t0);
Texture2D RenderInfor[4] : register(t1); //Position, Normal+ObjectID, Texture, Depth
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
	float4 cColor = RenderInfor[2][int2(input.position.xy)]; // 240 ~ 290 FPS
	//float4 cColor = RenderInfor[2].Sample(gssDefaultSamplerState, input.uv); // 210 ~ 240 FPS
	//float4 cColor = RenderInfor[2].Load(uint3((uint)input.position.x, (uint)input.position.y, 0)); // 280 ~ 320 FPS

	int Edge = false;
	float fObjectID = RenderInfor[1][int2(input.position.xy)].a;
	for (int i = 0; i < LineSize; i++)
	{
		if (RenderInfor[1][int2(input.position.xy) + gnOffsets[i]].a != 0 && fObjectID != 0)
			if (fObjectID != RenderInfor[1][int2(input.position.xy) + gnOffsets[i]].a) Edge = true; // 오브젝트 별 테두리
	}

	if (Edge)
		return(float4(LineColor));
	else
		return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////


struct VS_DEBUG_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	uint num : NUMBER;
};

VS_DEBUG_OUTPUT VSDebug(uint nVertexID : SV_VertexID)
{
	VS_DEBUG_OUTPUT output = (VS_DEBUG_OUTPUT)0;
	int s = nVertexID / 6;

	if (nVertexID % 6 == 0) { output.position = float4((0.5 * s) - 1.0f, -0.5f, 0.0f, 1.0f);		output.uv = float2(0.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 1) { output.position = float4((0.5 * (s + 1)) - 1.0f, -0.5f, 0.0f, 1.0f);	output.uv = float2(1.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 2) { output.position = float4((0.5 * (s + 1)) - 1.0f, -1.0f, 0.0f, 1.0f);	output.uv = float2(1.0f, 1.0f); output.num = s; }
	else if (nVertexID % 6 == 3) { output.position = float4((0.5 * s) - 1.0f, -0.5f, 0.0f, 1.0f);		output.uv = float2(0.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 4) { output.position = float4((0.5 * (s + 1)) - 1.0f, -1.0f, 0.0f, 1.0f);	output.uv = float2(1.0f, 1.0f); output.num = s; }
	else if (nVertexID % 6 == 5) { output.position = float4((0.5 * s) - 1.0f, -1.0f, 0.0f, 1.0f);		output.uv = float2(0.0f, 1.0f); output.num = s; }

	return(output);
}

float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_Target
{

	float4 cColor;
	if (input.num != 3) cColor = RenderInfor[input.num].Sample(gssDefaultSamplerState, input.uv);
	else cColor = RenderInfor[3].Sample(gssDefaultSamplerState, input.uv).r;
	return(cColor);

}


#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

SamplerState gssWrap : register(s0);


struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};
VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;
	matrix m = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.Scene = float4(0, 1, 0, 1);
	output.Position = float4(input.positionW, 1.0f);
	
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
	output.Texture = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	
	float3 normalW;
	
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
	}
	else
	{
		normalW = normalize(input.normalW);
	}
	
	output.Normal = float4(normalW.xyz, 1 / ((float)objectID + 2));
	return(output);
}

float4 PSBlend(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtAlbedoTexture.Sample(gssDefaultSamplerState, input.uv);

	return(cColor);
}


#define MAX_VERTEX_INFLUENCES			4
#define SKINNED_ANIMATION_BONES			256


cbuffer cbBoneOffsets : register(b7)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	//output.positionW = float3(0.0f, 0.0f, 0.0f);
	//output.normalW = float3(0.0f, 0.0f, 0.0f);
	//output.tangentW = float3(0.0f, 0.0f, 0.0f);
	//output.bitangentW = float3(0.0f, 0.0f, 0.0f);
	//matrix mtxVertexToBoneWorld;
	//for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	//{
	//	mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	//	output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	//	output.normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld);
	//	output.tangentW += input.weights[i] * mul(input.tangent, (float3x3)mtxVertexToBoneWorld);
	//	output.bitangentW += input.weights[i] * mul(input.bitangent, (float3x3)mtxVertexToBoneWorld);
	//}
	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		//		mtxVertexToBoneWorld += input.weights[i] * gpmtxBoneTransforms[input.indices[i]];
		mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	}

	output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;

	//	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}