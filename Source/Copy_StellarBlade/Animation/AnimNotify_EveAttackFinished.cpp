// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_EveAttackFinished.h"
#include "Components/SBStateComponent.h"
#include "Character/EveCharacter.h"

UAnimNotify_EveAttackFinished::UAnimNotify_EveAttackFinished(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotify_EveAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AEveCharacter* Character = Cast<AEveCharacter>(MeshComp->GetOwner()))
	{
		Character->AttackFinished(ComboResetDelay);
	}
}
