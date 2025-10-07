// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_SelectBehavior.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Components/SBCombatComponent.h"
#include "Animation/Monster_AnimInstance.h"

UBTService_SelectBehavior::UBTService_SelectBehavior()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTService_SelectBehavior::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	ControlledEnemy = Cast<AMonsterCharacter>(ControlledPawn);
}

void UBTService_SelectBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UpdateBehavior(OwnerComp);
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, EMonsterAIBehavior Behavior) const
{
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, bool boolValue) const
{
	BlackboardComp->SetValueAsBool(CanAttackKey.SelectedKeyName, boolValue);
}

void UBTService_SelectBehavior::UpdateBehavior(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	check(BlackboardComp)
		check(ControlledEnemy)

		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));

	// 매 프레임 경계 여부를 false로 설정
	UMonster_AnimInstance* AnimInstance = Cast< UMonster_AnimInstance>(OwnerComp.GetAIOwner()->GetCharacter()->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetIsHarassing(false);
	}

	if (IsValid(TargetActor))
	{
		SetBehaviorKey(BlackboardComp, EMonsterAIBehavior::Attack);
	}
	else
	{
		if (ControlledEnemy->GetPatrolPoint() != nullptr)
		{
			SetBehaviorKey(BlackboardComp, EMonsterAIBehavior::Patrol);
		}
		else
		{
			SetBehaviorKey(BlackboardComp, EMonsterAIBehavior::Idle);
		}
	}
}
