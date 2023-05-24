////////////////////////////////////////////////////////////////////////////
struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};


cbuffer cbWorldInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
	float value : packoffset(c4);
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
	matrix gmtxObjectWorld : packoffset(c0);
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
	uint LineSize : packoffset(c1.x);
	uint ToonShading : packoffset(c1.y);
};

Texture2DArray gtxtTextureArray : register(t0);
Texture2D RenderInfor[4] : register(t1); //Position, Normal+ObjectID, Texture, Depth
SamplerState gssDefaultSamplerState : register(s0);

Texture2D gtxtUITexture : register(t24);
SamplerState gssWrap : register(s0);
SamplerState gssBorder : register(s1);
// SkyBox
TextureCube gtxtSkyCubeTexture : register(t13);
#include "Light1.hlsl"

uint gnRenderMode = 0x10;
#define DYNAMIC_TESSELLATION		0x10
#define DEBUG_TESSELLATION			0x20
///////////////////////////////////////////////////////////////////////////////////////////

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 Scene : SV_TARGET0;
	float4 Position : SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 Texture : SV_TARGET3;
};

///////////////////////////////////////////////////////////////////////////////////////////
struct VS_BoundingINPUT
{
	float3 position : POSITION;
};

struct VS_BoundingOUTPUT
{
	float4 position : SV_POSITION;
};

VS_BoundingOUTPUT VSBounding(VS_BoundingINPUT input)
{
	VS_BoundingOUTPUT output;
	output.position = (mul(mul(mul(float4(input.position, 1.0f), gmtxObjectWorld), gmtxView), gmtxProjection));
	return(output);
}

float4 PSBounding(VS_BoundingOUTPUT input) : SV_TARGET
{
	if (objectID == 0) // 맵 오브젝트
	return(float4(1,1,1,1));
	if (objectID == 1) // 아군 플레이어
	return(float4(0, 1, 0, 1));
	if (objectID == 2) // 적
	return(float4(1, 0, 0, 1));
	if (objectID == 3) // 아군 공격
		return(float4(0, 0, 1, 1));
	if (objectID == 4) // 맵 Sphere 오브젝트
		return(float4(0.5, 0.5, 0.5, 1));
	if (objectID == 5) // 적 공격
		return(float4(0.0, 0.0, 0.0, 1));
	else return(float4(0, 0, 0, 1));
}

///////////////////////////////////////////////////////////////////////////////////////////

struct VS_BillboardINPUT
{
	float3 position : POSITION;
};

struct VS_BillboardOUTPUT
{
	float4 positionW : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_BillboardOUTPUT VSBillboard(VS_BillboardINPUT input, uint nVertexID : SV_VertexID)
{
	VS_BillboardOUTPUT output;
	if (nVertexID == 0)		 { output.uv = float2(-(1/value-1), 0.0f); }
	else if (nVertexID == 1) { output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.uv = float2(-(1 / value - 1), 1.0f); }
	else if (nVertexID == 3) { output.uv = float2(-(1 / value - 1), 1.0f); }
	else if (nVertexID == 4) { output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 5) { output.uv = float2(1.0f, 1.0f); }

	output.positionW = (mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection));
	return(output);
}

float4 PSBillboard(VS_BillboardOUTPUT input) : SV_TARGET
{
	return(gtxtUITexture.Sample(gssBorder, input.uv));

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

	if		(nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 1.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 1.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 1.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
																	  
	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 1.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 1.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 1.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };

float4 PSScreen(VS_SCREEN_OUTPUT input) : SV_Target
{
	int Edge = false;
	float fObjectID = RenderInfor[1][int2(input.position.xy)].a;
	for (int i = 0; i < LineSize; i++)
	{
			if (fObjectID != RenderInfor[1][int2(input.position.xy) + gnOffsets[i]].a) Edge = true; // 오브젝트 별 테두리
	}

	float4 cColor = RenderInfor[2][int2(input.position.xy)] * Lighting(RenderInfor[0][int2(input.position.xy)], RenderInfor[1][int2(input.position.xy)], gf3CameraDirection);
	if (Edge)
		return (LineColor);

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

	if (nVertexID % 6 == 0)		 { output.position = float4((0.5 * s) - 1.0f, 1.0f, 0.0f, 1.0f);		output.uv = float2(0.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 1) { output.position = float4((0.5 * (s + 1)) - 1.0f, 1.0f, 0.0f, 1.0f);	output.uv = float2(1.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 2) { output.position = float4((0.5 * (s + 1)) - 1.0f, 0.5f, 0.0f, 1.0f);	output.uv = float2(1.0f, 1.0f); output.num = s; }
	else if (nVertexID % 6 == 3) { output.position = float4((0.5 * s) - 1.0f, 1.0f, 0.0f, 1.0f);		output.uv = float2(0.0f, 0.0f); output.num = s; }
	else if (nVertexID % 6 == 4) { output.position = float4((0.5 * (s + 1)) - 1.0f, 0.5f, 0.0f, 1.0f);	output.uv = float2(1.0f, 1.0f); output.num = s; }
	else if (nVertexID % 6 == 5) { output.position = float4((0.5 * s) - 1.0f, 0.5f, 0.0f, 1.0f);		output.uv = float2(0.0f, 1.0f); output.num = s; }

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
	output.positionW = mul(float4(input.position, 1.0f), gmtxObjectWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxObjectWorld);
	output.tangentW = mul(input.tangent, (float3x3)gmtxObjectWorld);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxObjectWorld);
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
	output.Texture = (cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor);
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

	output.Normal = float4(normalW.xyz, ((float)objectID / ((float)objectID + 2)));
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

	//	output.positionW = mul(float4(input.position, 1.0f), gmtxObjectWorld).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

//////////////////////////////////////////////////////////////////////////////////////
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = (mul(mul(mul(float4(input.position, 1.0f), gmtxObjectWorld), gmtxView), gmtxProjection)).xyww;
	output.positionL = input.position;

	return(output);
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssDefaultSamplerState, input.positionL);

	return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////


struct VS_UI_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_UI_OUTPUT VSUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output = (VS_UI_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(0.0f, 1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(1.0f, 1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(1.0f, 0.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 3) { output.position = float4(0.0f, 1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(1.0f, 0.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(0.0f, 0.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	output.position = mul(output.position, gmtxWorld);

	return(output);
}
float4 PSUI(VS_UI_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtUITexture.Sample(gssWrap, input.uv);

	return(cColor);
}


/// ///////////////////////////////////////////////////////////////////////////////////////

Texture2D TFF_Terrain_Dirt_1A_D : register(t14);
Texture2D TFF_Terrain_Dirt_Road_1A_D : register(t15);
Texture2D TFF_Terrain_Earth_1A_D : register(t16);
Texture2D TFF_Terrain_Earth_2A_D : register(t17);
Texture2D TFF_Terrain_Earth_3A_D : register(t18);
Texture2D TFF_Terrain_Grass_1A_D : register(t19);
Texture2D TFF_Terrain_Grass_2A_D : register(t20);
Texture2D TFF_Terrain_Sand_1A_D : register(t21);


Texture2D SplatMap_0 : register(t22);
Texture2D SplatMap_1 : register(t23);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 normal : NORMAL;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxObjectWorld), gmtxView), gmtxProjection);
	output.positionW = mul(float4(input.position, 1.0f), gmtxObjectWorld).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;
	output.normal = input.normal;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.Scene = float4(0, 1, 0, 1);
	output.Position = float4(input.positionW,1.0f);
	output.Normal = float4(input.normal,0.0f);

	float4 SplatMap0 = SplatMap_0.Sample(gssWrap, input.uv0);
	float4 SplatMap1 = SplatMap_1.Sample(gssWrap, input.uv0);
	float4 Dirt = TFF_Terrain_Dirt_1A_D.Sample(gssWrap, input.uv1);
	float4 Dirt_Road = TFF_Terrain_Dirt_Road_1A_D.Sample(gssWrap, input.uv1);
	float4 Earth_1 = TFF_Terrain_Earth_1A_D.Sample(gssWrap, input.uv1);
	float4 Earth_2 = TFF_Terrain_Earth_2A_D.Sample(gssWrap, input.uv1);
	float4 Earth_3 = TFF_Terrain_Earth_3A_D.Sample(gssWrap, input.uv1);
	float4 Grass_1 = TFF_Terrain_Grass_1A_D.Sample(gssWrap, input.uv1);
	float4 Grass_2 = TFF_Terrain_Grass_2A_D.Sample(gssWrap, input.uv1);
	float4 Sand = TFF_Terrain_Sand_1A_D.Sample(gssWrap, input.uv1);

	float4 cColor = float4(0,0,0,0);

	cColor += Grass_1 * SplatMap0.r;
	cColor += Grass_2 * SplatMap0.g;
	cColor += Earth_1 * SplatMap0.b;
	cColor += Earth_2 * SplatMap0.a;

	cColor += Earth_3 * SplatMap1.r;
	cColor += Dirt * SplatMap1.g;
	cColor += Sand * SplatMap1.b;
	cColor += Dirt_Road * SplatMap1.a;

	output.Texture = cColor;
	return output;
}

/////////////////////////////////////////////////////////////////////////////////////////

//struct VS_TESSTERRAIN_OUTPUT
//{
//	float3 position : POSITION;
//	float3 positionW : POSITION1;
//	float4 color : COLOR;
//	float2 uv0 : TEXCOORD0;
//	float2 uv1 : TEXCOORD1;
//	float3 normal : NORMAL;
//};
//
//
//VS_TESSTERRAIN_OUTPUT VSTessTerrain(VS_TERRAIN_INPUT input)
//{
//	VS_TESSTERRAIN_OUTPUT output;
//
//	output.position = input.position;
//	output.positionW = mul(float4(input.position, 1.0f), gmtxObjectWorld).xyz;
//	output.color = input.color;
//	output.uv0 = input.uv0;
//	output.uv1 = input.uv1;
//	output.normal = input.normal;
//
//	return(output);
//}
//
//struct HS_CONSTANT
//{
//	float fTessEdges[4] : SV_TessFactor;
//	float fTessInsides[2] : SV_InsideTessFactor;
//};
//
//struct HS_TERRAIN_OUTPUT
//{
//	float3 position : POSITION; 
//	float3 positionW : POSITION1;
//	float4 color : COLOR;
//	float2 uv0 : TEXCOORD0;
//	float2 uv1 : TEXCOORD1;
//	float3 normal : NORMAL;
//};
//
//struct DS_TERRAIN_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float3 positionW : POSITION;
//	float4 color : COLOR;
//	float2 uv0 : TEXCOORD0;
//	float2 uv1 : TEXCOORD1;
//	float3 normal : NORMAL;
//	float4 tessellation : TEXCOORD2;
//};
//
//[domain("quad")]
//[partitioning("integer")]
//[outputtopology("triangle_cw")]
//[outputcontrolpoints(25)]
//[patchconstantfunc("HSTerrainConstant")]
//[maxtessfactor(64.0f)]
//HS_TERRAIN_OUTPUT HSTerrain(InputPatch<VS_TESSTERRAIN_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
//{
//	HS_TERRAIN_OUTPUT output;
//
//	output.position = input[i].position;
//	output.positionW = input[i].positionW;
//	output.color = input[i].color;
//	output.uv0 = input[i].uv0;
//	output.uv1 = input[i].uv1;
//	output.normal = input[i].normal;
//
//	return(output);
//}
//
//
float CalculateTessFactor(float3 f3Position)
{
	float fDistToCamera = distance(f3Position, gf3CameraPosition);
	float s = saturate((fDistToCamera - 10.0f) / (300.0f - 10.0f));

	return(lerp(64.0f, 1.0f, s));
}
//
//HS_CONSTANT HSTerrainConstant(InputPatch<VS_TESSTERRAIN_OUTPUT, 25> input)
//{
//	HS_CONSTANT output;
//
//	float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
//	float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
//	float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
//	float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);
//
//	output.fTessEdges[0] = CalculateTessFactor(e0);
//	output.fTessEdges[1] = CalculateTessFactor(e1);
//	output.fTessEdges[2] = CalculateTessFactor(e2);
//	output.fTessEdges[3] = CalculateTessFactor(e3);
//
//	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
//	for (int i = 0; i < 25; i++)
//	{
//		f3Sum += input[i].positionW;
//	}
//	float3 f3Center = f3Sum / 25.0f;
//	output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);
//
//	return(output);
//}
//
//void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
//{
//	float tInv = 1.0f - t;
//	fBernstein[0] = tInv * tInv * tInv * tInv;
//	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
//	fBernstein[2] = 6.0f * t * t * tInv * tInv;
//	fBernstein[3] = 4.0f * t * t * t * tInv;
//	fBernstein[4] = t * t * t * t;
//}
//float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_OUTPUT, 25> patch, float uB[5], float vB[5])
//{
//	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
//	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
//	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
//	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
//	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
//	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);
//
//	return(f3Sum);
//}
//
//[domain("quad")]
//DS_TERRAIN_OUTPUT DSTerrain(HS_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_OUTPUT, 25> patch)
//{
//	DS_TERRAIN_OUTPUT output = (DS_TERRAIN_OUTPUT)0;
//
//	float uB[5], vB[5];
//	BernsteinCoeffcient5x5(uv.x, uB);
//	BernsteinCoeffcient5x5(uv.y, vB);
//
//	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
//	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
//	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);
//
//	float3 position = CubicBezierSum5x5(patch, uB, vB);
//	matrix mtxWorldViewProjection = mul(mul(gmtxWorld, gmtxView), gmtxProjection);
//	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);
//
//	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);
//
//	return(output);
//}
//
//
//PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTessTerrain(DS_TERRAIN_OUTPUT input) : SV_TARGET
//{
//	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
//	output.Scene = float4(0, 1, 0, 1);
//	output.Position = float4(input.positionW,1.0f);
//	output.Normal = float4(input.normal,0.0f);
//
//	float4 SplatMap0 = SplatMap_0.Sample(gssWrap, input.uv0);
//	float4 SplatMap1 = SplatMap_1.Sample(gssWrap, input.uv0);
//	float4 Dirt = TFF_Terrain_Dirt_1A_D.Sample(gssWrap, input.uv1);
//	float4 Dirt_Road = TFF_Terrain_Dirt_Road_1A_D.Sample(gssWrap, input.uv1);
//	float4 Earth_1 = TFF_Terrain_Earth_1A_D.Sample(gssWrap, input.uv1);
//	float4 Earth_2 = TFF_Terrain_Earth_2A_D.Sample(gssWrap, input.uv1);
//	float4 Earth_3 = TFF_Terrain_Earth_3A_D.Sample(gssWrap, input.uv1);
//	float4 Grass_1 = TFF_Terrain_Grass_1A_D.Sample(gssWrap, input.uv1);
//	float4 Grass_2 = TFF_Terrain_Grass_2A_D.Sample(gssWrap, input.uv1);
//	float4 Sand = TFF_Terrain_Sand_1A_D.Sample(gssWrap, input.uv1);
//
//	float4 cColor = float4(0,0,0,0);
//
//	cColor += Grass_1 * SplatMap0.r;
//	cColor += Grass_2 * SplatMap0.g;
//	cColor += Earth_1 * SplatMap0.b;
//	cColor += Earth_2 * SplatMap0.a;
//
//	cColor += Earth_3 * SplatMap1.r;
//	cColor += Dirt * SplatMap1.g;
//	cColor += Sand * SplatMap1.b;
//	cColor += Dirt_Road * SplatMap1.a;
//	output.Texture = cColor;
//	return output;
//}



struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 tessellation : TEXCOORD2;
};

[domain("quad")]
//[partitioning("fractional_even")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("HSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;

	return(output);
}


HS_TERRAIN_TESSELLATION_CONSTANT HSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	if (gnRenderMode & DYNAMIC_TESSELLATION)
	{
		float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
		float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
		float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
		float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);

		output.fTessEdges[0] = CalculateTessFactor(e0);
		output.fTessEdges[1] = CalculateTessFactor(e1);
		output.fTessEdges[2] = CalculateTessFactor(e2);
		output.fTessEdges[3] = CalculateTessFactor(e3);

		float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 25; i++) f3Sum += input[i].positionW;
		float3 f3Center = f3Sum / 25.0f;
		output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);
	}
	else
	{
		output.fTessEdges[0] = 20.0f;
		output.fTessEdges[1] = 20.0f;
		output.fTessEdges[2] = 20.0f;
		output.fTessEdges[3] = 20.0f;

		output.fTessInsides[0] = 20.0f;
		output.fTessInsides[1] = 20.0f;
	}

	return(output);
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);

	return(f3Sum);
}

[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxWorld, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);

	return(output);
}

float4 PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (gnRenderMode & (DEBUG_TESSELLATION | DYNAMIC_TESSELLATION))
	{
		if (input.tessellation.w <= 5.0f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 10.0f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 20.0f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 30.0f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 40.0f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 50.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 55.0f) cColor = float4(0.2f, 0.2f, 0.72f, 1.0f);
		else if (input.tessellation.w <= 60.0f) cColor = float4(0.5f, 0.75f, 0.75f, 1.0f);
		else cColor = float4(0.87f, 0.17f, 1.0f, 1.0f);
	}
	else
	{
		float4 SplatMap0 = SplatMap_0.Sample(gssWrap, input.uv0);
		float4 SplatMap1 = SplatMap_1.Sample(gssWrap, input.uv0);
		float4 Dirt = TFF_Terrain_Dirt_1A_D.Sample(gssWrap, input.uv1);
		float4 Dirt_Road = TFF_Terrain_Dirt_Road_1A_D.Sample(gssWrap, input.uv1);
		float4 Earth_1 = TFF_Terrain_Earth_1A_D.Sample(gssWrap, input.uv1);
		float4 Earth_2 = TFF_Terrain_Earth_2A_D.Sample(gssWrap, input.uv1);
		float4 Earth_3 = TFF_Terrain_Earth_3A_D.Sample(gssWrap, input.uv1);
		float4 Grass_1 = TFF_Terrain_Grass_1A_D.Sample(gssWrap, input.uv1);
		float4 Grass_2 = TFF_Terrain_Grass_2A_D.Sample(gssWrap, input.uv1);
		float4 Sand = TFF_Terrain_Sand_1A_D.Sample(gssWrap, input.uv1);

		float4 cColor = float4(0, 0, 0, 0);

		cColor += Grass_1 * SplatMap0.r;
		cColor += Grass_2 * SplatMap0.g;
		cColor += Earth_1 * SplatMap0.b;
		cColor += Earth_2 * SplatMap0.a;

		cColor += Earth_3 * SplatMap1.r;
		cColor += Dirt * SplatMap1.g;
		cColor += Sand * SplatMap1.b;
		cColor += Dirt_Road * SplatMap1.a;

	}

	return(cColor);
}
