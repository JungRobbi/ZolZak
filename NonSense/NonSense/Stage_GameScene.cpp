#include "Stage_GameScene.h"
#include "BoxCollideComponent.h"
#include "GameFramework.h"
class GameFramework;

void Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 700);
	LoadAllTextures(pd3dDevice, pd3dCommandList);
	LoadAllModels(pd3dDevice, pd3dCommandList);
	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	Aim* aim = new Aim(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Option_UI* m_Option_Dec_UI = new Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Line_Left_UI* m_Line_Left_UI = new Line_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Line_Right_UI* m_Line_Right_UI = new Line_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Color_Left_UI* m_Color_Left_UI = new Color_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Color_Right_UI* m_Color_Right_UI = new Color_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Toon_Left_UI* m_Toon_Left_UI = new Toon_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Toon_Right_UI* m_Toon_Right_UI = new Toon_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Mouse_Left_UI* m_Mouse_Left_UI = new Mouse_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Mouse_Right_UI* m_Mouse_Right_UI = new Mouse_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Shadow_Left_UI* m_Shadow_Left_UI = new Shadow_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Shadow_Right_UI* m_Shadow_Right_UI = new Shadow_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	ScriptUI = new NPCScript(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	GameOver_UI* m_GameOver_UI = new GameOver_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Loading_UI* m_Loading_UI = new Loading_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Stat_UI* m_Stat_UI = new Stat_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	ATKs = new ATK_UI * [14];
	DEFs = new DEF_UI * [14];
	for (int i = 0; i < 14; ++i)
	{
		ATKs[i] = new ATK_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
		DEFs[i] = new DEF_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
		ATKs[i]->SetMyPos(0.1 + 0.035 * (i % 7), 0.93 - 0.05*(i / 7), 0.035, 0.05);
		DEFs[i]->SetMyPos(0.1 + 0.035 * (i % 7), 0.78 - 0.05*(i / 7), 0.035, 0.05);
	}

	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, 37, 37, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(-16.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("asdf");
		StartNPC->script.emplace_back("�dffd�");
		StartNPC->script.emplace_back("�ffff���");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(139.85f, m_pTerrain->GetHeight(139.85f, 13.24f), 13.24f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, ModelMap["Skull_Close_Weapon"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-1000.f, -1000.f, 1000.f);
	TempObject->SetNum(11201);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, ModelMap["Skull_Far_Weapon"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-1000.f, -1000.f, 1000.f);
	TempObject->SetNum(11202);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, ModelMap["Orc_Close_Weapon"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-1000.f, -1000.f, 1000.f);
	TempObject->SetNum(11101);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, ModelMap["Orc_Far_Weapon"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-1000.f, -1000.f, 1000.f);
	TempObject->SetNum(11102);

	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], ModelMap["Goblin_Close_Weapon_L"], ModelMap["Goblin_Close_Weapon_R"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-8.1f, m_pTerrain->GetHeight(-8.1f, 91.15f), 91.15f);
	TempObject->SetNum(11001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], ModelMap["Goblin_Far_Weapon_L"], ModelMap["Goblin_Far_Weapon_R"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-2.23f, m_pTerrain->GetHeight(2.23f, 85.3f), 85.3f);
	TempObject->SetNum(11002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(19.42f, m_pTerrain->GetHeight(19.42f, 80.73f), 80.73f);
	TempObject->SetNum(11003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(-1.27f, m_pTerrain->GetHeight(-1.27f, 60.32f), 60.32f);
	TempObject->SetNum(11004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-0.14f, m_pTerrain->GetHeight(-0.14f, 36.66f), 36.66f);
	TempObject->SetNum(11005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(15.05f, m_pTerrain->GetHeight(15.05f, 26.07f), 26.07f);
	TempObject->SetNum(11006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(25.01f, m_pTerrain->GetHeight(25.01f, 42.33f), 42.33f);
	TempObject->SetNum(11007);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(53.29f, m_pTerrain->GetHeight(53.29f, 45.72f), 45.72f);
	TempObject->SetNum(11008);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(93.69f, m_pTerrain->GetHeight(93.69f, 34.69f), 34.69f);
	TempObject->SetNum(11009);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(100.43f, m_pTerrain->GetHeight(100.43f, 31.01f), 31.01f);
	TempObject->SetNum(11010);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(97.29f, m_pTerrain->GetHeight(97.29f, 30.13f), 30.13f);
	TempObject->SetNum(11011);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(40.59f, 5.4f, -29.63f);
	TempObject->SetNum(10000);
	TempObject->Rotate(0.f, 30.f, 0.f);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 40, 40);
	TempObject->SetPosition(91.f, 2.98f, 151.f);
	TempObject->SetNum(10000);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -67.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -107);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -147.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -27.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(199.f, 2.0f, -27.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(199.f, 2.0f, 13.f);
	TempObject->SetNum(10000);
	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };


	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_4_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_4_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_2_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_2_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/NonBlend_Props_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Objects_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_3_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_3_Map.bin", m_pBlendShader);

	m_pDaySkyBox = new DaySkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pDaySkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pNightSkyBox = new NightSkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pNightSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSunsetSkyBox = new SunsetSkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSunsetSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pSkyBox = m_pDaySkyBox;

	/*Item* m_Def = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 0);
	m_Def->SetPosition(-15.0f, 0.5, 103.0f);
	m_Def->ObjectID = 100;
	Item* m_Atk = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 1);
	m_Atk->SetPosition(-16.5f, 0.5, 103.0f);
	m_Atk->ObjectID = 101;
	Item* m_HP = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 2);
	m_HP->SetPosition(-17.0f, 0.5, 103.0f);
	m_HP->ObjectID = 102;
	CreateItemList.emplace_back(100, 0, -16.0f, 0.5, 103.0f);*/

	Item* m_Eye = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 3);
	m_Eye->SetPosition(-17.5f, 0.5, 103.0f);

	Item* m_Ear = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 4);
	m_Ear->SetPosition(-18.0f, 0.5, 103.0f);

	Item* m_Hand = new Item(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 5);
	m_Hand->SetPosition(-18.5f, 0.5, 103.0f);

	Signal_UI* m_SignalR = new Signal_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_SignalR->SetPosition(m_Eye->GetPosition());
	Signal_UI* m_SignalL = new Signal_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_SignalL->SetPosition(m_Eye->GetPosition());
	m_SignalL->Rotate(0, 90, 0);

	XMFLOAT3 p = { 0,0,0 };
	MainBGM = new Sound("Sound/TestMusic.mp3", FMOD_2D | FMOD_LOOP_NORMAL, &p);
	MainBGM->SetVolume(0.2);
	AddSound(MainBGM);

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}

void Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);

	while (false == CreateItemList.empty()) {
		auto iteminfo = CreateItemList.front();

		Item* item = new Item(GameFramework::MainGameFramework->GetDevice(), pd3dCommandList, m_pGraphicsRootSignature, iteminfo.ItemID);
		item->SetPosition(iteminfo.x, iteminfo.y, iteminfo.z);
		item->ObjectID = iteminfo.ItemNum;
		CreateItemList.pop_front();
	}
}

bool Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:

			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_DELETE:
			if (Boss) {
				if (NetworkMGR::b_isNet) {
					CS_TEMP_HIT_MONSTER_PACKET send_packet;
					send_packet.size = sizeof(CS_TEMP_HIT_MONSTER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_MONSTER_PACKET;
					send_packet.monster_id = ((Character*)Boss)->GetNum();
					send_packet.hit_damage = 20000;
					PacketQueue::AddSendPacket(&send_packet);
				}
				else {
					((Character*)Boss)->GetHit(20000);
				}
			}
			break;
		default:
			break;
		}

	default:
		break;
	}
	return(false);
}

void Stage_GameScene::TouchStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Sound_Debuff(-1);
	if (m_pPlayer)
		m_pPlayer->SetPosition(XMFLOAT3(226.04f, m_pTerrain->GetHeight(226.04f, 46.f), 46.f));
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(228.04f, m_pTerrain->GetHeight(228.04f, 48.f) - 10.5f, 48.f);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("�ȳ�");
		StartNPC->script.emplace_back("���� �������� ����");
		StartNPC->script.emplace_back("���� ���� ������");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(-29.25f, m_pTerrain->GetHeight(-29.25f, 146.89f), 146.89f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(206.1f, m_pTerrain->GetHeight(206.1f, 82.9f), 82.9f);
	TempObject->SetNum(10201);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"],NULL,NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(172.9f, m_pTerrain->GetHeight(172.9f, 71.6f), 71.6f);
	TempObject->SetNum(10202);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(190.1f, m_pTerrain->GetHeight(190.1f, 128.8f), 128.8f);
	TempObject->SetNum(10203);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(166.62f, m_pTerrain->GetHeight(166.62f, 142.3f), 142.3f);
	TempObject->SetNum(10204);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(141.4f, m_pTerrain->GetHeight(141.4f, 156.7f), 156.7f);
	TempObject->SetNum(10205);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(126.5f, m_pTerrain->GetHeight(126.5f, 184.9f), 184.9f);
	TempObject->SetNum(10206);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(99.f, m_pTerrain->GetHeight(99.f, 171.5f), 171.5f);
	TempObject->SetNum(10207);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(74.1f, m_pTerrain->GetHeight(74.1f, 162.1f), 162.1f);
	TempObject->SetNum(10208);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(45.9f, m_pTerrain->GetHeight(45.9f, 153.3f), 153.3f);
	TempObject->SetNum(10209);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(153.3f, m_pTerrain->GetHeight(153.3f, 146.6f), 146.6f);
	TempObject->SetNum(10210);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(154.9f, m_pTerrain->GetHeight(154.9f, 151.8f), 151.8f);
	TempObject->SetNum(10211);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(161.9f, m_pTerrain->GetHeight(161.9f, 128.8f), 128.8f);
	TempObject->SetNum(10212);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkyBox = m_pNightSkyBox;
}

void Stage_GameScene::HearingStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pPlayer)
		m_pPlayer->SetPosition(XMFLOAT3(125.31, m_pTerrain->GetHeight(125.31, 18.39), 18.39));
	Sound_Debuff(5.0f);
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(139.85f, m_pTerrain->GetHeight(139.85f, 13.24f), 13.24f);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("�ȳ�");
		StartNPC->script.emplace_back("���� �������� ����");
		StartNPC->script.emplace_back("���� ���� ������");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(285.f, m_pTerrain->GetHeight(285.f, 43.f) + 1.5f, 43.f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL,NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-149.62f, m_pTerrain->GetHeight(-149.62f, 8.02f), 8.02f);
	TempObject->SetNum(10101);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(-159.09f, m_pTerrain->GetHeight(-159.09f, 31.32f), 31.32f);
	TempObject->SetNum(10102);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(176.4f, m_pTerrain->GetHeight(176.4f, 42.8f), 42.8f);
	TempObject->SetNum(10103);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(195.55f, m_pTerrain->GetHeight(195.55f, 50.82f), 50.82f);
	TempObject->SetNum(10104);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(226.1f, m_pTerrain->GetHeight(226.1f, 53.4f), 53.4f);
	TempObject->SetNum(10105);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(247.1f, m_pTerrain->GetHeight(247.1f, 31.1f), 31.1f);
	TempObject->SetNum(10106);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(280.7, m_pTerrain->GetHeight(280.7, 20.2), 20.2);
	TempObject->SetNum(10107);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(273.1f, m_pTerrain->GetHeight(273.1f, -12.6f), -12.6f);
	TempObject->SetNum(10108);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(256.9f, m_pTerrain->GetHeight(256.9f, -37.3f), -37.3f);
	TempObject->SetNum(10109);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(215.f, m_pTerrain->GetHeight(215.f, -52.21f), -52.21f);
	TempObject->SetNum(10110);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(196.f, m_pTerrain->GetHeight(196.f, -46.3f), -46.3f);
	TempObject->SetNum(10111);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(176.2f, m_pTerrain->GetHeight(176.2f, -25.f), -25.f);
	TempObject->SetNum(10112);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(178.5f, m_pTerrain->GetHeight(178.5f, -48.3f), -48.3f);
	TempObject->SetNum(10113);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(184.9f, m_pTerrain->GetHeight(184.9f, -70.3f), -70.3f);
	TempObject->SetNum(10114);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkyBox = m_pSunsetSkyBox;
}

void Stage_GameScene::SightStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Sound_Debuff(-1);
	if (m_pPlayer)
		m_pPlayer->SetPosition(XMFLOAT3( -21.73f, m_pTerrain->GetHeight(-21.73f, 108.9f), 108.9f));
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(-16.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("asdf");
		StartNPC->script.emplace_back("�dffd�");
		StartNPC->script.emplace_back("�ffff���");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(139.85f, m_pTerrain->GetHeight(139.85f, 13.24f), 13.24f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-8.1f, m_pTerrain->GetHeight(-8.1f, 91.15f), 91.15f);
	TempObject->SetNum(10001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(-2.23f, m_pTerrain->GetHeight(2.23f, 85.3f), 85.3f);
	TempObject->SetNum(10002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(19.42f, m_pTerrain->GetHeight(19.42f, 80.73f), 80.73f);
	TempObject->SetNum(10003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(-1.27f, m_pTerrain->GetHeight(-1.27f, 60.32f), 60.32f);
	TempObject->SetNum(10004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-0.14f, m_pTerrain->GetHeight(-0.14f, 36.66f), 36.66f);
	TempObject->SetNum(10005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(15.05f, m_pTerrain->GetHeight(15.05f, 26.07f), 26.07f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(25.01f, m_pTerrain->GetHeight(25.01f, 42.33f), 42.33f);
	TempObject->SetNum(10007);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(53.29f, m_pTerrain->GetHeight(53.29f, 45.72f), 45.72f);
	TempObject->SetNum(10008);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(93.69f, m_pTerrain->GetHeight(93.69f, 34.69f), 34.69f);
	TempObject->SetNum(10009);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(100.43f, m_pTerrain->GetHeight(100.43f, 31.01f), 31.01f);
	TempObject->SetNum(10010);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(97.29f, m_pTerrain->GetHeight(97.29f, 30.13f), 30.13f);
	TempObject->SetNum(10011);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(40.59f, 5.4f, -29.63f);
	TempObject->SetNum(10000);
	TempObject->Rotate(0.f, 30.f, 0.f);
	m_pSkyBox = m_pDaySkyBox;
}

void Stage_GameScene::BossStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pPlayer)
		m_pPlayer->SetPosition(XMFLOAT3(-145.1f, m_pTerrain->GetHeight(-145.1f, 174.98f), 174.98f));
	Sound_Debuff(-1);
	Object* TempObject = NULL;
	TempObject = new Shield(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Boss_Shield"], NULL, NULL, MONSTER_TYPE_BOSS);
	TempObject->SetPosition(-177.75f, m_pTerrain->GetHeight(-177.75f, 173.79f), 173.79f);
	TempObject->SetNum(22222);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	Boss = TempObject;
	m_pSkyBox = m_pDaySkyBox;
}

void Stage_GameScene::ClearMonster()
{
	if (&MonsterObjects) {
		for (auto& object : MonsterObjects)
			delete object;
		MonsterObjects.clear();
	}
	Boss = NULL;
	if (gameObjects.size() > 0)
	{
		auto iter = std::find(gameObjects.begin(), gameObjects.end(), StartNPC);
		gameObjects.erase(iter);
		iter = std::find(gameObjects.begin(), gameObjects.end(), EndNPC);
		gameObjects.erase(iter);
	}
}

void Stage_GameScene::LoadAllModels(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadedModelInfo* Model = NULL;
	
	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Boss_Shield.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Boss_Shield", Model));
	if(Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if(Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();


	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Ent.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Ent", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();


	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/F05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("F05", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();


	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close_Weapon_L", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close_Weapon_R", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();


	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far_Weapon_L", Model));

	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far_Weapon_R", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	//Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush.bin", NULL);
	//ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush", Model));


	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush_Weapon_L", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush_Weapon_R", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/M05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("M05", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Close", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Close_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Close_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Far", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Far_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Rush.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Rush", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Rush_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Rush_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Close", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Close_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Close_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Far", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Far_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Rush.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Rush", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Rush_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Rush_Weapon", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Sword_M05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Sword_M05", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Wand.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Wand", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Wand_F05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Wand_F05", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Far_Weapon_Fire", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();
	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far_Weapon_R_Fire", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();
	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Far_Weapon_Fire", Model));
	if (Model->m_pRoot)
		Model->m_pRoot->AddRef();
	if (Model->m_pAnimationSets)
		Model->m_pAnimationSets->AddRef();
}

void Stage_GameScene::LoadAllTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CTexture* tex = NULL;
	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Ent_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Ent_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/F05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("F05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Face.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Face", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/face02.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("face02", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/face03.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("face03", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Goblin_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Goblin_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Goblin_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Goblin_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Goblin_4.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Goblin_4", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Hair05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Hair05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/HairF05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("HairF05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/M05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("M05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Orc.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Orc", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Orc_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Orc_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Orc_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Orc_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Orc_4.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Orc_4", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Shield_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Shield_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Skin.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Skin", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Skull_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Skull_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Skull_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Skull_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Skull_4.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("Skull_4", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/SwordM05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("SwordM05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Atlas_1A_D.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Atlas_1A_D", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Atlas_Billboards_1A_D.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Atlas_Billboards_1A_D", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Atlas_Lights_1A_D.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Atlas_Lights_1A_D", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Atlas_Vegetation_1A_D.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Atlas_Vegetation_1A_D", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Flower_Patch_01A.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Flower_Patch_01A", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Flower_Patch_01B.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Flower_Patch_01B", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Flower_Patch_01C.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Flower_Patch_01C", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Glass_Gradient_1A.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Glass_Gradient_1A", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Gradient_Mask_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Gradient_Mask_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Grass_Patch_01A.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Grass_Patch_01A", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Grass_Patch_02A.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Grass_Patch_02A", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Grass_Patch_02C.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Grass_Patch_02C", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Day_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Day_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Day_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Day_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Day_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Day_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Day_4.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Day_4", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Dusk_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Dusk_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_LUT_Night_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_LUT_Night_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Noise_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Noise_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Toon_Ramp_1A.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Toon_Ramp_1A", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Toon_Ramp_1B.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Toon_Ramp_1B", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Toon_Ramp_1C.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Toon_Ramp_1C", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/TFF_Water_Normal_1.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("TFF_Water_Normal_1", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/WandF05.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("WandF05", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/weapon_2.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("weapon_2", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/weapon_3.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("weapon_3", tex));

	tex = new CTexture(1, RESOURCE_TEXTURE2D, 0, 8);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/weapon_4.dds", RESOURCE_TEXTURE2D, 0);
	GameScene::CreateShaderResourceViews(pd3dDevice, tex, 8, false);
	TextureMap.insert(std::pair<std::string, CTexture*>("weapon_4", tex));


}

void Stage_GameScene::update()
{
	GameScene::update();

	if (BossDead)
	{
		if (SceneChangeCount > 0)
		{
			SceneChangeCount -= Timer::GetTimeElapsed();
		}
		else
		{
			BossDead = false;
			SceneChangeCount = 5;
			Die = false;
			if (NetworkMGR::b_isNet) {
				CS_CLEAR_PACKET send_packet;
				send_packet.size = sizeof(CS_CLEAR_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_CLEAR_PACKET;
				send_packet.ClearScene = (int)BOSS_SCENE;
				PacketQueue::AddSendPacket(&send_packet);
			}
			else {
				GameFramework::MainGameFramework->ChangeScene(LOBBY_SCENE);
			}
		}
	}

	if (GameFramework::MainGameFramework->scene_type == HEARING_SCENE)
	{
		if (HearingDebuffTime <= 0.0f)
		{
			Sound_Debuff(5.0f);
			HearingDebuffTime = 10.0f;
		}
		else
		{
			HearingDebuffTime -= Timer::GetTimeElapsed();
		}
	}

}

void Stage_GameScene::RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender(pd3dCommandList, pCamera);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	if (m_pMappedScreenOptions->darkness <= 0.5) {
		for (auto& object : MonsterObjects)
		{
			object->m_pHP->UpdateTransform(NULL);
			object->m_pHP->Render(pd3dCommandList, pCamera);;
		}
	}
	for (auto& object : UIGameObjects)
	{
		object->UpdateTransform(NULL);
		object->Render(pd3dCommandList, pCamera);
	}
	if (ScriptMode)
	{
		ScriptUI->UpdateTransform(NULL);
		ScriptUI->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < 14; ++i)
	{
		if (m_pPlayer->m_Attack >= 200 + 20 * i)
		{
			ATKs[i]->UpdateTransform(NULL);
			ATKs[i]->Render(pd3dCommandList, pCamera);
		}
		if (m_pPlayer->m_Defense >= 100 + 10 * i)
		{
			DEFs[i]->UpdateTransform(NULL);
			DEFs[i]->Render(pd3dCommandList, pCamera);
		}
	}
}
