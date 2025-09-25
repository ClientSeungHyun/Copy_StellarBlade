// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_SelectBehavior.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MonsterCharacter.h"

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

	UpdateBehavior(OwnerComp.GetBlackboardComponent());
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, ESBAIBehavior Behavior) const
{
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::UpdateBehavior(UBlackboardComponent* BlackboardComp) const
{
	check(BlackboardComp)
	check(ControlledEnemy)

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));

	if (IsValid(TargetActor))
	{
		const float Distance = TargetActor->GetDistanceTo(ControlledEnemy);

		// 공격범위 안쪽이면
		if (Distance <= AttackRangeDistance)
		{
			SetBehaviorKey(BlackboardComp, ESBAIBehavior::MeleeAttack);
		}
		else
		{
			SetBehaviorKey(BlackboardComp, ESBAIBehavior::Approach);
		}
	}
	else
	{
		// Patrol point 있으면
		if (ControlledEnemy->GetPatrolPoint() != nullptr)
		{
			SetBehaviorKey(BlackboardComp, ESBAIBehavior::Patrol);
		}
		else
		{
			SetBehaviorKey(BlackboardComp, ESBAIBehavior::Idle);
		}
	}
}
