// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_EveComboWindow.h"
#include "Character/EveCharacter.h"

UAnimNotifyState_EveComboWindow::UAnimNotifyState_EveComboWindow(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotifyState_EveComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AEveCharacter* Character = Cast<AEveCharacter>(MeshComp->GetOwner()))
	{
		Character->EnableComboWindow();
	}
}

void UAnimNotifyState_EveComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AEveCharacter* Character = Cast<AEveCharacter>(MeshComp->GetOwner()))
	{
		Character->DisableComboWindow();
	}
}
