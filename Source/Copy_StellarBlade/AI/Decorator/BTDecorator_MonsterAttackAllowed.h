// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_MonsterAttackAllowed.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UBTDecorator_MonsterAttackAllowed : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector TargetBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RangeMin = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RangeMax = 200.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector CanAttackBlackboardKey;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
