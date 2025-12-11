// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EveEffect.generated.h"

/**
 * 
 */
UCLASS()
class COPY_STELLARBLADE_API UAnimNotify_EveEffect : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	bool sword_lightning_On = false;

	UPROPERTY(EditAnywhere)
	bool sword_lightning_Off = false;

	UPROPERTY(EditAnywhere)
	bool start_Dodge = false;

	UPROPERTY(EditAnywhere)
	bool End_Overlay_effect = false;	
	
	UPROPERTY(EditAnywhere)
	bool start_Blink_overlay = false;	
	
	UPROPERTY(EditAnywhere)
	bool start_body_light = false;		
	
	UPROPERTY(EditAnywhere)
	bool end_body_light = false;	
	
	UPROPERTY(EditAnywhere)
	bool sword_blink = false;


protected:
	UPROPERTY()
	class AEveCharacter* Character = nullptr;



public:
	UAnimNotify_EveEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 노티파이 발생 시 실행
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};