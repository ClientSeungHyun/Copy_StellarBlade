// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_UpdateMoveSpeed.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskNode_UpdateMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	if (UCharacterMovementComponent* MovementComp = ControlledPawn->GetComponentByClass< UCharacterMovementComponent>())
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BlackboardLocation.SelectedKeyName);

		MovementComp->MaxWalkSpeed = MaxWalkSpeed;

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
