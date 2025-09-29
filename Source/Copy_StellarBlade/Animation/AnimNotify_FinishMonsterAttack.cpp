// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FinishMonsterAttack.h"
#include "Interfaces/SBCombatInterface.h"

void UAnimNotify_FinishMonsterAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (ISBCombatInterface* CombatInterface = Cast<ISBCombatInterface>(OwnerActor))
		{
			CombatInterface->FinishAttack();
		}
	}
}
