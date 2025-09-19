// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SBCombatComponent.generated.h"

class ASBWeapon;

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnChangedCombat, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COPY_STELLARBLADE_API USBCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Combat 상태 전환을 알리는 Delegate */
	FDelegateOnChangedCombat OnChangedCombat;

protected:
	UPROPERTY()
	ASBWeapon* MainWeapon;

	/* 전투 활성화 상태인지? */
	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;

	/** 마지막 AttackType */
	UPROPERTY(VisibleAnywhere)
	FGameplayTag LastAttackType;

public:
	USBCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetWeapon(ASBWeapon* NewWeapon);

public:
	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	void SetCombatEnabled(const bool bEnabled);

	FORCEINLINE ASBWeapon* GetMainWeapon() const { return MainWeapon; };

	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType; };
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackTypeTag) { LastAttackType = NewAttackTypeTag; };

		
};
