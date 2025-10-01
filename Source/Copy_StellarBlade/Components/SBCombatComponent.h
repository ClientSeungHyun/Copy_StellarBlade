// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SBCombatComponent.generated.h"

class ASBWeapon;
class AAIController;

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

	UPROPERTY()
	ASBWeapon* SubWeapon;

	AAIController* OwnerAIController = nullptr;

	/* 전투 활성화 상태인지? */
	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;

	UPROPERTY(EditAnywhere)
	bool bHaveTarget = false;

	/** 마지막 AttackType */
	UPROPERTY(VisibleAnywhere)
	FGameplayTag LastAttackType;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	bool bCanAttack = false;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float MinAttackTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float MaxAttackTime = 5.f;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	float AttackDelayTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float FollowStartRange = 400.f;

public:
	USBCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetWeapon(ASBWeapon* NewWeapon, bool isSubWeapon = false);
	void FinishAttack();

public:
	FORCEINLINE bool IsCombatEnabled() const { return MainWeapon && bCombatEnabled; }
	void SetCombatEnabled(const bool bEnabled);

	FORCEINLINE bool IsHaveTarget() const { return bHaveTarget; }
	void SetHaveTarget(const bool bEnabled);

	FORCEINLINE ASBWeapon* GetMainWeapon() const { return MainWeapon; };
	FORCEINLINE ASBWeapon* GetSubWeapon() const { return SubWeapon; };

	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType; };
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackTypeTag) { LastAttackType = NewAttackTypeTag; };

	FORCEINLINE float GetFollowStartRange() { return FollowStartRange; };

	//델리게이트
public:
	void OnChangedTarget(const bool bInHaveTarget);
};
