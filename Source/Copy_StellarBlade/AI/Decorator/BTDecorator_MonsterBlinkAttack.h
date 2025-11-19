// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_MonsterBlinkAttack.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UBTDecorator_MonsterBlinkAttack : public UBTDecorator
{
	GENERATED_BODY()
	

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
