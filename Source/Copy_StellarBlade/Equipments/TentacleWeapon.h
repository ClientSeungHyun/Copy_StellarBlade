// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/SBWeapon.h"
#include "TentacleWeapon.generated.h"


UCLASS()
class COPY_STELLARBLADE_API ATentacleWeapon : public ASBWeapon
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere);
	TArray<FName> Array_TraceStartEndName;

public:
	ATentacleWeapon();

public:
	virtual void BeginPlay() override;

public:
	virtual void EquipItem(bool isSubWeapon = false) override;
};
