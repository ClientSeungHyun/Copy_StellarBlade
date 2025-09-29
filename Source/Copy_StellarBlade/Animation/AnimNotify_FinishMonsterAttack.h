// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FinishMonsterAttack.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotify_FinishMonsterAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	//UAnimNotify_FinishMonsterAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
