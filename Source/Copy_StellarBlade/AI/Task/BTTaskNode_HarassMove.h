// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_HarassMove.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UBTTaskNode_HarassMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float MinDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float DesiredDistance = 300.f; // 적정 거리

public:
	FVector CalculateHarassDirection(UBehaviorTreeComponent& OwnerComp);
};
