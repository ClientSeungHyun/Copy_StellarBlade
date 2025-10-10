// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_EveWeaponColl.h"
#include "Character//EveCharacter.h"
#include "Player/SBEveWeapon.h"

UAnimNotifyState_EveWeaponColl::UAnimNotifyState_EveWeaponColl(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimNotifyState_EveWeaponColl::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (AEveCharacter* Eve = Cast<AEveCharacter>(OwnerActor))
		{
			ASBEveWeapon* Sword = Eve->GetWeapon();
			if (::IsValid(Sword))
			{
				Sword->ActivateCollision();
			}
		}
	}
}

void UAnimNotifyState_EveWeaponColl::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (AEveCharacter* Eve = Cast<AEveCharacter>(OwnerActor))
		{
			ASBEveWeapon* Sword = Eve->GetWeapon();
			if (::IsValid(Sword))
			{
				Sword->DeactivateCollision();
			}
		}
	}
}
