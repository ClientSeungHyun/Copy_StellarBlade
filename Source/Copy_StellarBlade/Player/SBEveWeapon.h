// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SBEveWeapon.generated.h"

UCLASS()
class COPY_STELLARBLADE_API ASBEveWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	/** °Ë ¸Þ½¬ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	class USBMontageActionData* MontageActionData;

public:	
	ASBEveWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
};
