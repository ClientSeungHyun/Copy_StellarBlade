// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_MonsterBlinkAttack.h"

#include "Character/Monster/MonsterCharacter.h"
#include "AI/MonsterAIController.h"

#include "Kismet/KismetSystemLibrary.h"

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

		if (AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(OwnerMonster->GetController()))
		{
			MonsterAIController->SetIsLookingPlayer(false);
		}

		UKismetSystemLibrary::PrintString(
			this,
			FString::Printf(TEXT("지금이야!")),
			true,
			true,
			FLinearColor::Green,
			2.0f
		);
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
