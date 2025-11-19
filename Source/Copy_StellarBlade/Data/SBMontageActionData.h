// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SBMontageActionData.generated.h"

USTRUCT(BlueprintType)
struct FSBMontageGroup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> Animations;
};


UCLASS()
class COPY_STELLARBLADE_API USBMontageActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Montage Groups")
	TMap<FGameplayTag, FSBMontageGroup> MontageGroupMap;

public:
	UAnimMontage* GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index) const;
	UAnimMontage* GetRandomMontageForTag(const FGameplayTag& GroupTag) const;

	bool		IsHaveMontageForTag(const FGameplayTag& GroupTag) const;
};
