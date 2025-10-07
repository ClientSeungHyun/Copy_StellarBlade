// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MovePosition.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotifyState_MovePosition : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    float MoveDistance = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    FVector MoveDirection = FVector::ZeroVector;

    FVector StartLocation = FVector::ZeroVector;

    float MoveSpeed = 100.f;
    float ElapsedTime = 0.f;
    float MoveDuration = 0.f;

};
