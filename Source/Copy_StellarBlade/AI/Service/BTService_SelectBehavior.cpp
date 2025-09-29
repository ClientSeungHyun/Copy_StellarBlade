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

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, EMonsterAIBehavior Behavior) const
{
	BlackboardComp->SetValueAsEnum(BehaviorKey.SelectedKeyName, static_cast<uint8>(Behavior));
}

void UBTService_SelectBehavior::SetBehaviorKey(UBlackboardComponent* BlackboardComp, bool boolValue) const
{
	BlackboardComp->SetValueAsBool(CanAttackKey.SelectedKeyName, boolValue);
}

void UBTService_SelectBehavior::UpdateBehavior(UBlackboardComponent* BlackboardComp) const
{
	check(BlackboardComp)
	check(ControlledEnemy)

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));

	if (IsValid(TargetActor))
	{
		const float Distance = TargetActor->GetDistanceTo(ControlledEnemy);
		const bool bCanAttack = BlackboardComp->GetValueAsBool(CanAttackKey.SelectedKeyName);

		if (bCanAttack)
		{
			SetBehaviorKey(BlackboardComp, EMonsterAIBehavior::Attack);
		}
		else
		{
			SetBehaviorKey(BlackboardComp, EMonsterAIBehavior::Harass);
		}
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
