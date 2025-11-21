// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_MonsterBlinkAttack.h"

#include "AIController.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Equipments/SBWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/MonsterAIController.h"

bool UBTDecorator_MonsterBlinkAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AMonsterCharacter* OwnerMonster = Cast<AMonsterCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	const bool isHaveBlinkAttack = OwnerMonster->IsHaveBlinkAttack();
	
	const int8 CurrentAttackCount = OwnerMonster->GetCurrentAttackCount();
	const int8 BlinkAttackCount = OwnerMonster->GetBlinkAttackCount();

	if (CurrentAttackCount >= BlinkAttackCount && isHaveBlinkAttack)
		return true;

	return false;
}