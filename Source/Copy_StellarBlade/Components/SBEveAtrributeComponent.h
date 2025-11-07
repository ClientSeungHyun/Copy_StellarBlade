// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Components/ActorComponent.h"
#include "SBEveAtrributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EAttributeType, float);
DECLARE_MULTICAST_DELEGATE(FOnDeath);

class AEveCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COPY_STELLARBLADE_API USBEveAtrributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** 스탯 변경 Delegate */
	FDelegateOnAttributeChanged OnAttributeChanged;
	/** 죽음을 알리는 Delegate */
	FOnDeath OnDeath;

protected:
	UPROPERTY(EditAnywhere, Category = "Health")
	float BaseHealth = 200;

	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 200;

	UPROPERTY(EditAnywhere, Category = "Beta")
	float CurrentBetaEnergy = 0.f;

	UPROPERTY(EditAnywhere, Category = "Beta")
	float MaxBetaEnergy = 100.f;

	UPROPERTY(EditAnywhere, Category = "Potion")
	int32 MaxPotionCount= 3;

	UPROPERTY(EditAnywhere, Category = "Potion")
	int32 CurrentPotionCount = MaxPotionCount;

private:
	float PotionHealAmount = 20.f;
	float SkillUseBetaEnergyAmout = 40.f;

private:
	AEveCharacter* OwnerCharacter = nullptr;

public:	
	USBEveAtrributeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	FORCEINLINE float GetBaseHealth() const { return BaseHealth; };
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; };
	FORCEINLINE float GetHealthRatio() const { return BaseHealth / MaxHealth; };
	int32 GetCurrentPotionCount() const { return CurrentPotionCount; };

	/** 스텟 변경을 통지하는 Broadcast Function */
	void BroadcastAttributeChanged(EAttributeType InAttributeType) const;
	void TakeDamageAmount(float DamageAmount);
	
	float GetBetaEnergy() const { return CurrentBetaEnergy; };
	void AddBetaEnergy(float num = 10);
	void UsePotion();
	void DecreaseBetaEnergy();
};
