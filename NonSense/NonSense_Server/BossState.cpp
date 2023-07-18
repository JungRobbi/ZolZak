#include "BossState.h"
#include "Characters.h"
#include "Components/BossAttackComponent.h"
#include <random>

static std::uniform_real_distribution<float> RandomSkillCoolTime(10.0f, 15.0f);
static std::uniform_int_distribution<int> RandomSkill(0, 4);
static std::uniform_real_distribution<float> RandomIdleTime(3.0, 8.0f);
static std::uniform_real_distribution<float> RandomPosDistance(-5.0f, 5.0f);
static std::random_device rd;
static std::default_random_engine dre(rd());


WanderState_Boss* WanderState_Boss::GetInstance()
{
	static WanderState_Boss state;
	return &state;
}

void WanderState_Boss::Enter(BossFSMComponent* pOwner)
{
	//std::cout << "Start Wandering" << std::endl;
	pOwner->Stop();
	XMFLOAT3 OwnerPos = pOwner->GetOwnerPosition();
	float NewPosx = OwnerPos.x + RandomPosDistance(dre);
	float NewPosz = OwnerPos.z + RandomPosDistance(dre);
	pOwner->ResetWanderPosition(NewPosx, NewPosz);
}

void WanderState_Boss::Execute(BossFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Boss::GetInstance());
	}

	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Boss::GetInstance());
	}
	if (pOwner->Wander())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Boss::GetInstance());
	}
}

void WanderState_Boss::Exit(BossFSMComponent* pOwner)
{
	//std::cout << "Stop Wandering" << std::endl;
}

TrackEnemyState_Boss* TrackEnemyState_Boss::GetInstance()
{
	static TrackEnemyState_Boss state;
	return &state;
}

void TrackEnemyState_Boss::Enter(BossFSMComponent* pOwner)
{
	//std::cout << "Start Tracking" << std::endl;

}

void TrackEnemyState_Boss::Execute(BossFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Boss::GetInstance());
	}
	pOwner->Track();
	if (!pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Boss::GetInstance());
	}
	if (pOwner->GetSkillCoolTime() <= 0.0)
	{
		pOwner->GetFSM()->ChangeState(SkillState_Boss::GetInstance());
	}

}

void TrackEnemyState_Boss::Exit(BossFSMComponent* pOwner)
{
	//std::cout << "Stop Tracking" << std::endl;
}

IdleState_Boss* IdleState_Boss::GetInstance()
{
	static IdleState_Boss state;
	return &state;
}

void IdleState_Boss::Enter(BossFSMComponent* pOwner)
{
	//std::cout << "Start Idle" << std::endl;
	pOwner->Stop();
	pOwner->ResetIdleTime(RandomIdleTime(rd));
}

void IdleState_Boss::Execute(BossFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Boss::GetInstance());
	}
	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Boss::GetInstance());
	}
	if (pOwner->Idle())
	{
		pOwner->GetFSM()->ChangeState(WanderState_Boss::GetInstance());
	}
}

void IdleState_Boss::Exit(BossFSMComponent* pOwner)
{
	//std::cout << "Stop Idel" << std::endl;
}

DeathState_Boss* DeathState_Boss::GetInstance()
{
	static DeathState_Boss state;
	return &state;
}

void DeathState_Boss::Enter(BossFSMComponent* pOwner)
{
	//std::cout << "Unit Die" << std::endl;
}

void DeathState_Boss::Execute(BossFSMComponent* pOwner)
{
	
}

void DeathState_Boss::Exit(BossFSMComponent* pOwner)
{
}

SkillState_Boss* SkillState_Boss::GetInstance()
{
	static SkillState_Boss state;
	return &state;
}

void SkillState_Boss::Enter(BossFSMComponent* pOwner)
{
	pOwner->Stop();
	std::cout << "Boss Skill" << std::endl;
}

void SkillState_Boss::Execute(BossFSMComponent* pOwner)
{
	if (pOwner->IsTornado)
	{
		pOwner->TornadoTrack();
	}
	if (pOwner->gameObject->GetComponent<BossAttackComponent>()->End_Skill)
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Boss::GetInstance());
		pOwner->IsTornado = false;
		return;
	}
	if (!pOwner->gameObject->GetComponent<BossAttackComponent>()->During_Skill)
	{
		int Skill = RandomSkill(rd);
		std::cout << "SKill : " << Skill << std::endl;
		switch (Skill)
		{
		case 0:
			pOwner->StealSense();
			break;

		case 1:
			pOwner->Summon();
			break;

		case 2:
			pOwner->Defence();
			break;

		case 3:
			pOwner->JumpAttack();
			break;

		case 4:
			pOwner->Tornado();
			pOwner->IsTornado = true;
			break;
		default:
			break;
		}
	}


}

void SkillState_Boss::Exit(BossFSMComponent* pOwner)
{
	pOwner->SetSkillCoolTime(RandomSkillCoolTime(rd));
	pOwner->gameObject->GetComponent<BossAttackComponent>()->End_Skill = false;
	pOwner->gameObject->GetComponent<BossAttackComponent>()->During_Skill = false;
	std::cout << "Boss Skill End" << std::endl;

}
