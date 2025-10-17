// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_MonsterBlinkAttack.h"

#include "Character/Monster/MonsterCharacter.h"

UAnimNotifyState_MonsterBlinkAttack::UAnimNotifyState_MonsterBlinkAttack(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotifyState_MonsterBlinkAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AMonsterCharacter* OwnerMonster = Cast<AMonsterCharacter>(MeshComp->GetOwner()))
	{
		OwnerMonster->SetAllowCounterAttack_Blink(true);
	}
}

void UAnimNotifyState_MonsterBlinkAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AMonsterCharacter* OwnerMonster = Cast<AMonsterCharacter>(MeshComp->GetOwner()))
	{
		OwnerMonster->SetAllowCounterAttack_Blink(false);
	}
}
