// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:	
	ASBEveWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
