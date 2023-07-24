#include "Room_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

void Room_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 200);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);


	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;

	Room_Back_UI* Rm_oom_Back_UI = new Room_Back_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Mage_UI* m_Mage_UI = new Mage_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Warrior_UI* m_Warrior_UI = new Warrior_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Ready_UI* m_Ready_UI = new Ready_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Leave_Room_UI* m_Leave_Room_UI = new Leave_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
}
void Room_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Room_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Room_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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