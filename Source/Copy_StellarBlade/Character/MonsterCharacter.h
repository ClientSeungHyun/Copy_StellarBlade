// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/SBCombatInterface.h"
#include "MonsterCharacter.generated.h"


class USBStateComponent;
class USBAttributeComponent;
class ATargetPoint;
class USBCombatComponent;
class ASBWeapon;
class URotationComponent;
class UWidgetComponent;

UCLASS()
class COPY_STELLARBLADE_API AMonsterCharacter : public ACharacter, public ISBCombatInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	USBAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	USBStateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere)
	USBCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere)
	URotationComponent* RotationComponent;

	/** HealthBar */
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditAnywhere)
	float HealthBarOffsetPosY = 100.f;


	// Effect Section
protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ImpactParticle;

	// Montage Section
protected:
	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimFront;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimBack;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimLeft;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimRight;

protected:
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	TArray<ATargetPoint*> PatrolPoints;

	UPROPERTY(VisibleAnywhere, Category = "AI | Patrol")
	int32 PatrolIndex = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASBWeapon> DefaultWeaponClass;

public:
	AMonsterCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void OnDeath();
	void OnAttributeChanged(ESBAttributeType AttributeType, float InValue);
	void SetupHealthBar();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;

public:
	//CombatInterface 구현.
	virtual void ActivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void DeactivateWeaponCollision(EWeaponCollisionType WeaponCollisionType) override;
	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) override;

	// 체력바 토글
	void ToggleHealthBarVisibility(bool bVisibility);

public:
	FORCEINLINE ATargetPoint* GetPatrolPoint()
	{
		return PatrolPoints.Num() >= (PatrolIndex + 1) ? PatrolPoints[PatrolIndex] : nullptr;
	}
	FORCEINLINE void IncrementPatrolIndex()
	{
		PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	}
};
