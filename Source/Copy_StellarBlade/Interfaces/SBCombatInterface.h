// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SBGameplayTags.h"
#include "SBDefine.h"
#include "GameplayTagContainer.h"
#include "SBCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USBCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COPY_STELLARBLADE_API ISBCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) = 0;

	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) = 0;

	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) {};
};
