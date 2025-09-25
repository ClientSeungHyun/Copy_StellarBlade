// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SBDefine.h"
#include "AnimNotifyState_WeaponCollision.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotifyState_WeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::MainWeapon;

public:
	UAnimNotifyState_WeaponCollision(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
