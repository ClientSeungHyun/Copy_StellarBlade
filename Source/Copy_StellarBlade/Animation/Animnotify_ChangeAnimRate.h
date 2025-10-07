// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animnotify_ChangeAnimRate.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimnotify_ChangeAnimRate : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	// 변경할 재생 속도 (예: 0.5 = 느리게, 2.0 = 빠르게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage Speed")
	float NewPlayRate = 1.0f;


public:
	UAnimnotify_ChangeAnimRate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 노티파이 발생 시 실행
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};