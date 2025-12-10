// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "SBEveWeapon.generated.h"

class USBWeaponCollisionComponent;
class AEveCharacter;
struct FGameplayTag;

UCLASS()
class COPY_STELLARBLADE_API ASBEveWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	/** 검 메쉬 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Animation")
	class USBMontageActionData* MontageActionData;

	UPROPERTY(VisibleAnywhere)
	USBWeaponCollisionComponent* WeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> AttackSoundList;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* Guard_effects[3];

	UPROPERTY()
	UNiagaraComponent* GuardComps[3];

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* Blink_effect;

public:	
	ASBEveWeapon();

protected:
	/** 기본 데미지 */
	UPROPERTY(EditAnywhere)
	float BaseDamage = 15.f;

	/** 데미지 승수 */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inform")
	FVector PrevPosition = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Inform")
	FVector AttackDirection = FVector::ZeroVector;


private:
	FGameplayTag lastAttackTag;
	AEveCharacter* Owner = nullptr;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


private:
	void CalculateWeaponMoveDirection();

public:
	virtual void EquipItem();

	void SetLastAttackTag(FGameplayTag tag);
	FGameplayTag GetLastAttackTag() { return lastAttackTag; }

	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	FORCEINLINE USBWeaponCollisionComponent* GetCollision(int32 CollisionNum) const { return WeaponCollision; };

	float GetAttackDamage() ;

	void PlayAllGuardEffects();
	void StopAllGuardEffects();
	void PlayBlinkEffect();

	virtual void ActivateCollision();
	virtual void DeactivateCollision();

	void PlayAttackSound();

	/** 무기의 Collision에 검출된 Actor에 Damage를 전달 */
	void OnHitActor(const FHitResult& Hit);

public:
	FVector GetAttackDirection();
};
