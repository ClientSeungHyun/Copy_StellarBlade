// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootprintSound.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotify_FootprintSound : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	USoundBase* FootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	FName FootBoneName = "FootStepSound";

public:
	UAnimNotify_FootprintSound();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};