// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_MonsterAttackAllowed.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UBTDecorator_MonsterAttackAllowed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	const AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey.SelectedKeyName));
	if (!TargetActor)
	{
		return false;
	}

	// 설정한 거리 내 및 공격 가능 변수가 true라면
	const float Distance = ControlledPawn->GetDistanceTo(TargetActor);
	const bool bCanAttack = OwnerComp.GetBlackboardComponent()->GetValueAsBool(CanAttackBlackboardKey.SelectedKeyName);

	return UKismetMathLibrary::InRange_FloatFloat(Distance, RangeMin, RangeMax) && bCanAttack;
}