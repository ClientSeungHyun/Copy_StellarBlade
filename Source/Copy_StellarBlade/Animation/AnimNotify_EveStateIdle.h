// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EveStateIdle.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotify_EveStateIdle : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_EveStateIdle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 노티파이 발생 시 실행
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};