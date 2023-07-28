#include "stdafx.h"
#include "Player.h"
#include <algorithm>
#include "PlayerMovementComponent.h"
#include "AttackComponent.h"
#include "GameScene.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "../ImaysNet/PacketQueue.h"
#include "NetworkMGR.h"
#include "GameFramework.h"

Player::Player() : Object(false)
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

Player::~Player()
{
	m_pUI->Release();
	m_pHP_UI->Release();
	m_pHP_Dec_UI->Release();
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void Player::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Object::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void Player::ReleaseShaderVariables()
{
	Object::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}
void Player::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Object::UpdateShaderVariables(pd3dCommandList);
}


void Player::Move(ULONG dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		if (NetworkMGR::b_isNet) {
			XMFLOAT3 xmf3Dir = Vector3::Normalize(xmf3Shift);
			CS_MOVE_PACKET send_packet;
			send_packet.size = sizeof(CS_MOVE_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_MOVE;
			send_packet.dirX = xmf3Dir.x;
			send_packet.dirY = xmf3Dir.y;
			send_packet.dirZ = xmf3Dir.z;
			PacketQueue::AddSendPacket(&send_packet);
		}
		else {
			Move(xmf3Shift, bUpdateVelocity);
		}
	}
}
void Player::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}
void Player::Sight_DeBuff(float sec)
{
	last_DeBuff = Timer::GetTotalTime() + sec;
}
void Player::Rotate(float x, float y, float z)
{
	if (NetworkMGR::b_isNet) {
		CS_ROTATE_PACKET send_packet;
		send_packet.size = sizeof(CS_ROTATE_PACKET);
		send_packet.type = E_PACKET::E_PACKET_CS_ROTATE;
		send_packet.Add_Pitch = x;
		send_packet.Add_Yaw = y;
		send_packet.Add_Roll = z;
		PacketQueue::AddSendPacket(&send_packet);

		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
	}
	else {
		DWORD nCameraMode = m_pCamera->GetMode();
		if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
		{
			if (x != 0.0f)
			{
				m_fPitch += x;
				if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
				if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
			}
			if (y != 0.0f)
			{
				m_fYaw += y;
				if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
				if (m_fYaw < 0.0f) m_fYaw += 360.0f;
			}
			if (z != 0.0f)
			{
				m_fRoll += z;
				if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
				if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
			}
			m_pCamera->Rotate(x, y, z);

			if (y != 0.0f)
			{
				XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
				m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
				m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
			}
		}
		else if (nCameraMode == SPACESHIP_CAMERA)
		{
			m_pCamera->Rotate(x, y, z);
			if (x != 0.0f)
			{
				XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
					XMConvertToRadians(x));
				m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
				m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			}
			if (y != 0.0f)
			{
				XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
					XMConvertToRadians(y));
				m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
				m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
			}
			if (z != 0.0f)
			{
				XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look),
					XMConvertToRadians(z));
				m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
				m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
			}
		}

		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	}
}
void Player::Update(float fTimeElapsed)
{
	if (last_DeBuff >= Timer::GetTotalTime())
	{
		dark = true;
	}
	else {
		dark = false;
	}
	if (!NetworkMGR::b_isNet) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
		if (fLength > m_fMaxVelocityXZ)
		{
			m_xmf3Velocity.x *= (m_fMaxVelocityXZ / fLength);
			m_xmf3Velocity.z *= (m_fMaxVelocityXZ / fLength);
		}
		float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
		fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
		if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
		//GetComponent<SphereCollideComponent>()->Center.x += xmf3Velocity.x;
		//GetComponent<SphereCollideComponent>()->Center.y += xmf3Velocity.y;
		//GetComponent<SphereCollideComponent>()->Center.z += xmf3Velocity.z;S
		//GetComponent<SphereCollideComponent>()->update();
		//bool bound = false;

		//for (auto& o : GameScene::MainScene->gameObjects)
		//{
		//	if (o->GetComponent<BoxCollideComponent>())
		//	{
		//		if (GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*o->GetComponent<BoxCollideComponent>()->GetBoundingObject()))
		//		{
		//			bound = true;
		//			Move(XMFLOAT3(-xmf3Velocity.x, 0, -xmf3Velocity.z), false);
		//			break;
		//		}
		//	}
		//}
		Move(xmf3Velocity, false);
		//GetComponent<SphereCollideComponent>()->Center.x -= xmf3Velocity.x;
		//GetComponent<SphereCollideComponent>()->Center.y -= xmf3Velocity.y;
		//GetComponent<SphereCollideComponent>()->Center.z -= xmf3Velocity.z;

		fLength = Vector3::Length(m_xmf3Velocity);
		float fDeceleration = (m_fFriction * fTimeElapsed);
		if (fDeceleration > fLength) fDeceleration = fLength;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
		if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	}

	DWORD nCameraMode = m_pCamera->GetMode();
	//if (nCameraMode == THIRD_PERSON_CAMERA) 
	m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	Animate(fTimeElapsed);
	Object::update();
}

void Player::OnPlayerUpdateCallback(float fTimeElapsed)
{
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)m_pPlayerUpdatedContext;

	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x + 400.0f, xmf3PlayerPosition.z + 400.0f);
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void Player::OnCameraUpdateCallback(float fTimeElapsed)
{
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x + 400.0f, xmf3CameraPosition.z + 400.0f, bReverseQuad);
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			ThirdPersonCamera* p3rdPersonCamera = (ThirdPersonCamera*)m_pCamera;
			XMFLOAT3 pos = GetPosition();
			p3rdPersonCamera->SetLookAt(pos);
		}
	}
}

void Player::GetHit(float damage)
{
	m_RemainHP -= damage;
	if (m_RemainHP <= 0)
	{
		cout << "asdf";
	}
}
Camera* Player::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다.
	Camera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new FirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new ThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new SpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		XMFLOAT3 right = XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z);
		m_xmf3Right = Vector3::Normalize(right);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3Up = Vector3::Normalize(up);
		XMFLOAT3 look = XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z);
		m_xmf3Look = Vector3::Normalize(look);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		XMFLOAT3 angle = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_fYaw = Vector3::Angle(angle, m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{

		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}
	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;
	return(pNewCamera);
}

void Player::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x;
	m_xmf4x4ToParent._12 = m_xmf3Right.y;
	m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x;
	m_xmf4x4ToParent._22 = m_xmf3Up.y;
	m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x;
	m_xmf4x4ToParent._32 = m_xmf3Look.y;
	m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x;
	m_xmf4x4ToParent._42 = m_xmf3Position.y;
	m_xmf4x4ToParent._43 = m_xmf3Position.z;

	XMMATRIX mat = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mat, m_xmf4x4ToParent);

}
void Player::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{

	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	//	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		if (m_pMaterial) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		Object::Render(pd3dCommandList, pCamera);

	}

}


MagePlayer::MagePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	Magical = true;
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

	m_pHP_Dec_UI = new Player_HP_DEC_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHP_UI = new Player_HP_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pUI = new Player_State_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	fireball = new FireBall(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pHP_UI->SetParentUI(m_pUI);
	m_pHP_Dec_UI->SetParentUI(m_pUI);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.3);

	//CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	//BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BoundMesh, m_pBoundingShader);
	//bb->SetNum(3);
	//GetComponent<AttackComponent>()->SetBoundingObject(bb);

	AddComponent<PlayerMovementComponent>();
	AddComponent<AttackComponent>();
	GetComponent<AttackComponent>()->SetAttackDuration(1.5);
	{
		XMFLOAT3 pos;
		m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		CreateShaderVariables(pd3dDevice, pd3dCommandList);
		if (GameScene::MainScene->GetTerrain())
		{
			float h = GameScene::MainScene->GetTerrain()->GetHeight(-16.0f, 103.0f);
			cout << h << endl;
			pos = XMFLOAT3(-16.0f, h, 103.0f);
		}
		else
		{
			pos = XMFLOAT3(0, 0, 0);
		}
		SetPosition(pos);
		LoadedModelInfo* pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/F05.bin", NULL);
		LoadedModelInfo* pWeaponModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Wand.bin", NULL);

		if (pModel)
			SetChild(pModel->m_pRoot, true);
		if (pWeaponModel) {
			Object* Hand = FindFrame("Sword_parentR"); // 무기를 붙여줄 팔 찾기
			if (Hand) {
				CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.1f);
				BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BoundMesh, m_pBoundingShader);
				bb->SetNum(3);
				Hand->SetChild(pWeaponModel->m_pRoot, true);
				pWeaponObject = new Object(false);
				pWeaponObject->SetChild(pWeaponModel->m_pRoot, true);
				pWeaponObject->SetPosition(0, 5, 0);
				bb->SetPosition(pWeaponObject->FindFirstMesh()->GetBoundingBox().Center.x, pWeaponObject->FindFirstMesh()->GetBoundingBox().Center.y + pWeaponObject->FindFirstMesh()->GetBoundingBox().Extents.y,pWeaponObject->FindFirstMesh()->GetBoundingBox().Extents.z);
			}
		}

		SetNum(9);
		m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 3, pModel);
		m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
		m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);

		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[3]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[4]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[6]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[7]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[8]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[9]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[10]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[11]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[12]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[13]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[14]->m_nType = ANIMATION_TYPE_ONCE; 


		std::function<void()> FootStepREvent = [this]() {
			this->FootStepR();
		};
		std::function<void()> FootStepLEvent = [this]() {
			this->FootStepL();
		};
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepREvent", E_WALK, 0.2, FootStepREvent);
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepLEvent", E_WALK, 0.65, FootStepLEvent);

		m_pSkinnedAnimationController->AddAnimationEvent("FootStepREvent", E_RUN, 0.13, FootStepREvent);
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepLEvent", E_RUN, 0.33, FootStepLEvent);
	}
}

void MagePlayer::FootStepR()
{
	if (GameFramework::MainGameFramework->GameSceneState >= SIGHT_SCENE)
	{
		Sound* s = new Sound("Sound/GrassFootstep_R.mp3", false);
		GameScene::MainScene->AddSound(s);
	}
}

void MagePlayer::FootStepL()
{
	if (GameFramework::MainGameFramework->GameSceneState >= SIGHT_SCENE)
	{
		Sound* s = new Sound("Sound/GrassFootstep_L.mp3", false);
		GameScene::MainScene->AddSound(s);
	}
}

Camera* MagePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		//플레이어의 특성을 1인칭 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
		SetFriction(30.0f);
		SetGravity(XMFLOAT3(0.0f, -60.0f, 0.0f));
		SetMaxVelocityXZ(5.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0, 0.7f, 0.25));
		m_pCamera->GenerateProjectionMatrix(0.1f, 1000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		//플레이어의 특성을 스페이스-쉽 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
		SetFriction(50.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(100.0f);
		SetMaxVelocityY(100.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(0.01f, 1000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(30.0f);
		SetGravity(XMFLOAT3(0.0f, -60.0f, 0.0f));
		SetMaxVelocityXZ(5.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.5f, -3.0f));
		//m_pCamera->SetOffset(XMFLOAT3(0, 0.8f, 0.2));

		m_pCamera->GenerateProjectionMatrix(0.01f, 100.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);
	return(m_pCamera);
}

void MagePlayer::Update(float fTimeElapsed)
{
	Player::Update(fTimeElapsed);
	DWORD nCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	UpdateTransform(NULL);

	m_pHP_Dec_UI->Dec_HP = GetRemainHP() / GetHealth();
	m_pHP_UI->SetMyPos(0.2, 0.04, 0.8 * GetRemainHP() / GetHealth(), 0.32);

	if (nCameraMode == FIRST_PERSON_CAMERA && FindFrame("Face"))
	{

	}
}

void MagePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (GameScene::MainScene->m_pPlayer == this)
	{
		DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
		if (nCameraMode == FIRST_PERSON_CAMERA)
		{
			m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

			FindFrame("Wand")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Body_F05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Arm_F05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Leg_F05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
		}
		else
		{
			Player::Render(pd3dCommandList, pCamera);
		}
	}
	else
	{
		Player::Render(pd3dCommandList, pCamera);
	}
}



WarriorPlayer::WarriorPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	Magical = false;
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

	m_pHP_Dec_UI = new Player_HP_DEC_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHP_UI = new Player_HP_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pUI = new Warrior_Player_State_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	fireball = new FireBall(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_pHP_UI->SetParentUI(m_pUI);
	m_pHP_Dec_UI->SetParentUI(m_pUI);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.3);

	CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BoundMesh, m_pBoundingShader);
	bb->SetNum(3);
	AddComponent<PlayerMovementComponent>();
	AddComponent<AttackComponent>();
	GetComponent<AttackComponent>()->SetBoundingObject(bb);
	GetComponent<AttackComponent>()->SetAttackDuration(1);

	{
		XMFLOAT3 pos;
		m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		CreateShaderVariables(pd3dDevice, pd3dCommandList);
		if (GameScene::MainScene->GetTerrain())
		{
			float h = GameScene::MainScene->GetTerrain()->GetHeight(-16.0f, 103.0f);
			pos = XMFLOAT3(-16.0f, h, 103.0f);
		}
		else
		{
			pos = XMFLOAT3(0, 0, 0);
		}
		SetPosition(pos);
		LoadedModelInfo* pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/M05.bin", NULL);
		LoadedModelInfo* pWeaponModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Sword_M05.bin", NULL);

		if (pModel)
			SetChild(pModel->m_pRoot, true);
		if (pWeaponModel) {
			Object* Hand = FindFrame("Sword_parentR"); // 무기를 붙여줄 팔 찾기
			if (Hand) {
				CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 0.1f, 0.1f, 0.1f);
				BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BoundMesh, m_pBoundingShader);
				bb->SetNum(3);
				Hand->SetChild(pWeaponModel->m_pRoot, true);
				pWeaponObject = new Object(false);
				pWeaponObject->SetChild(pWeaponModel->m_pRoot, true);
				pWeaponObject->SetPosition(0, 5, 0);
				bb->SetPosition(pWeaponObject->FindFirstMesh()->GetBoundingBox().Center.x, pWeaponObject->FindFirstMesh()->GetBoundingBox().Center.y + pWeaponObject->FindFirstMesh()->GetBoundingBox().Extents.y, pWeaponObject->FindFirstMesh()->GetBoundingBox().Extents.z);
			}
		}

		SetNum(9);
		m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 3, pModel);
		m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
		m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);
		m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pSkinnedAnimationController->SetTrackEnable(2, false);

		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[3]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[4]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[6]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[7]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[8]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[9]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[10]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[11]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[12]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[13]->m_nType = ANIMATION_TYPE_ONCE;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[14]->m_nType = ANIMATION_TYPE_ONCE;

		std::function<void()> FootStepREvent = [this]() {
			this->FootStepR();
		};
		std::function<void()> FootStepLEvent = [this]() {
			this->FootStepL();
		};
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepREvent", E_WALK, 0.2, FootStepREvent);
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepLEvent", E_WALK, 0.65, FootStepLEvent);

		m_pSkinnedAnimationController->AddAnimationEvent("FootStepREvent", E_RUN, 0.13, FootStepREvent);
		m_pSkinnedAnimationController->AddAnimationEvent("FootStepLEvent", E_RUN, 0.33, FootStepLEvent);
	}
}

void WarriorPlayer::FootStepR()
{
	if (GameFramework::MainGameFramework->GameSceneState >= SIGHT_SCENE)
	{
		Sound* s = new Sound("Sound/GrassFootstep_R.mp3", false);
		GameScene::MainScene->AddSound(s);
	}
}

void WarriorPlayer::FootStepL()
{
	if (GameFramework::MainGameFramework->GameSceneState >= SIGHT_SCENE)
	{
		Sound* s = new Sound("Sound/GrassFootstep_L.mp3", false);
		GameScene::MainScene->AddSound(s);
	}
}
Camera* WarriorPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		//플레이어의 특성을 1인칭 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
		SetFriction(30.0f);
		SetGravity(XMFLOAT3(0.0f, -60.0f, 0.0f));
		SetMaxVelocityXZ(5.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0, 0.7f, 0.25));
		m_pCamera->GenerateProjectionMatrix(0.01f, 1000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		//플레이어의 특성을 스페이스-쉽 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다.
		SetFriction(50.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(100.0f);
		SetMaxVelocityY(100.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(0.01f, 1000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(30.0f);
		SetGravity(XMFLOAT3(0.0f, -60.0f, 0.0f));
		SetMaxVelocityXZ(5.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.5f, -3.0f));
		//m_pCamera->SetOffset(XMFLOAT3(0, 0.8f, 0.2));

		m_pCamera->GenerateProjectionMatrix(0.01f, 100.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);
	return(m_pCamera);
}

void WarriorPlayer::Update(float fTimeElapsed)
{
	Player::Update(fTimeElapsed);
	DWORD nCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	UpdateTransform(NULL);
	if (nCameraMode == FIRST_PERSON_CAMERA && FindFrame("Face"))
	{

	}
}

void WarriorPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (GameScene::MainScene->m_pPlayer == this)
	{
		DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
		if (nCameraMode == FIRST_PERSON_CAMERA)
		{
			m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

			FindFrame("Sword_M05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Body_m05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Arm_m05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
			FindFrame("Leg_M05")->RenderOnlyOneFrame(pd3dCommandList, pCamera);
		}
		else
		{
			Player::Render(pd3dCommandList, pCamera);
		}
	}
	else
	{
		Player::Render(pd3dCommandList, pCamera);
	}
}
