// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_EveWeaponColl.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotifyState_EveWeaponColl : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_EveWeaponColl(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
