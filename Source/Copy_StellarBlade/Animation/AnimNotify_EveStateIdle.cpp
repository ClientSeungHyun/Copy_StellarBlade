// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_EveStateIdle.h"
#include "Components/SBStateComponent.h"
#include "Character/EveCharacter.h"
#include "SBEveTags.h"

UAnimNotify_EveStateIdle::UAnimNotify_EveStateIdle(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotify_EveStateIdle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AEveCharacter* Character = Cast<AEveCharacter>(MeshComp->GetOwner()))
	{
		Character->GetStateComponent()->SetState(SBEveTags::Eve_State_Idle);
	}
}
