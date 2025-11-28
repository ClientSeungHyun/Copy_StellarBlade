// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlaySound_Once.generated.h"

UENUM(BlueprintType)
enum class ESoundType : uint8
{
    Footstep   UMETA(DisplayName = "Footstep"),
    Jump       UMETA(DisplayName = "Jump"),
    Land       UMETA(DisplayName = "Land"),
    Potion       UMETA(DisplayName = "Potion"),
    Dash       UMETA(DisplayName = "Dash"),
    SwordSwish       UMETA(DisplayName = "SwordSwish"),
};

UCLASS()
class COPY_STELLARBLADE_API UAnimNotify_PlaySound_Once : public UAnimNotify
{
	GENERATED_BODY()
	
public:
    UPROPERTY()
    USoundBase* FootstepSound;

    UPROPERTY()
    USoundBase* JumpSound;

    UPROPERTY()
    USoundBase* LandSound;

    UPROPERTY()
    USoundBase* PotionSound; 
    
    UPROPERTY()
    USoundBase* DashSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TArray<USoundBase*> SwordSoundList;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    int32 SelectedIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    ESoundType SoundType;

public:

    UAnimNotify_PlaySound_Once();

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
