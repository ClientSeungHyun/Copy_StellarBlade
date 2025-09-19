// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "GameplayTagContainer.h"
#include "Equipments/SBEquipment.h"
#include "SBWeapon.generated.h"

class USBWeaponCollisionComponent;
class USBMontageActionData;
struct FGameplayTag;
class USBCombatComponent;

UCLASS()
class COPY_STELLARBLADE_API ASBWeapon : public ASBEquipment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Socket")
	FName UnequipSocketName;

	// Combat Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	ECombatType CombatType = ECombatType::SwordShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	USBMontageActionData* MontageActionData;

	// Component Section
protected:
	UPROPERTY(VisibleAnywhere)
	USBWeaponCollisionComponent* WeaponCollision;

	UPROPERTY(VisibleAnywhere)
	USBWeaponCollisionComponent* SecondWeaponCollision;

protected:
	UPROPERTY()
	USBCombatComponent* CombatComponent;

	// Data Section
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap;

	/** 기본 데미지 */
	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	/** 데미지 승수 */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

public:
	ASBWeapon();


public:
	virtual void EquipItem() override;

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	UAnimMontage* GetRandomMontageForTag(const FGameplayTag& Tag) const;
	UAnimMontage* GetHitReactMontage(const AActor* Attacker) const;

	float GetStaminaCost(const FGameplayTag& InTag) const;
	float GetAttackDamage() const;

	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; };
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; };
	FORCEINLINE USBWeaponCollisionComponent* GetCollision() const { return WeaponCollision; };
	FORCEINLINE ECombatType GetCombatType() const { return CombatType; };

public:
	virtual void ActivateCollision(EWeaponCollisionType InCollisionType);
	virtual void DeactivateCollision(EWeaponCollisionType InCollisionType);

public:
	/** 무기의 Collision에 검출된 Actor에 Damage를 전달 */
	void OnHitActor(const FHitResult& Hit);
};
