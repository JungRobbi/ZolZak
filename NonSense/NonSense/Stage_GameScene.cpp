#include "Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

void Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	
}
void Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
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
		default:
			break;
		}

	default:
		break;
	}
	return(false);
}

void Stage_GameScene::LoadAllModels(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadedModelInfo* Model = NULL;
	
	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Boss_Shield.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Boss_Shield", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Ent.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Ent", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/F05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("F05", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close_Weapon_L", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Close_Weapon_R", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far_Weapon_L", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Far_Weapon_R", Model));

	//Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush.bin", NULL);
	//ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush_Weapon_L.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush_Weapon_L", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Rush_Weapon_R.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Goblin_Rush_Weapon_R", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/M05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("M05", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Close", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Close_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Close_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Far", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Far_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Rush.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Rush", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Orc_Rush_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Orc_Rush_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Close.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Close", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Close_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Close_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Far.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Far", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Far_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Far_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Rush.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Rush", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Skull_Rush_Weapon.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Skull_Rush_Weapon", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Sword_M05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Sword_M05", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Wand.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Wand", Model));

	Model = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Wand_F05.bin", NULL);
	ModelMap.insert(std::pair<std::string, LoadedModelInfo*>("Wand_F05", Model));

}
