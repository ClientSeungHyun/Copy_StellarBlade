// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_EveEffect.h"
#include "Character/EveCharacter.h"
#include "SBEveTags.h"

UAnimNotify_EveEffect::UAnimNotify_EveEffect(const FObjectInitializer& ObjectInitializer)
{
}

void UAnimNotify_EveEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (Character == nullptr )
	{
		Character = Cast<AEveCharacter>(MeshComp->GetOwner());
	}

	if (sword_lightning_On)
	{
		if(Character != nullptr)
			Character->ActiveSwordEffect();
	}

	if (sword_lightning_Off)
	{
		if (Character != nullptr)
			Character->DeActiveSwordEffect();
	}

	if (start_Dodge && Character != nullptr)
		Character->StartPerfectDodge();


	if (End_Overlay_effect && Character != nullptr)
		Character->EndOverlayEffect();	
	
	if (start_Blink_overlay && Character != nullptr)
		Character->StartBlinkOverlayEffect();	
	
	if (start_body_light && Character != nullptr)
		Character->StartBodyLightEffect();
	
	if (end_body_light && Character != nullptr)
		Character->EndBodyLightEffect();
	
	if (sword_blink && Character != nullptr)
		Character->StartWordBlinkEffct();

}