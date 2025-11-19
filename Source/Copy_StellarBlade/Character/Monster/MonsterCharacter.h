// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/SBCombatInterface.h"
#include "Interfaces/TargetingInterface.h"

#include "ProceduralMeshComponent.h"
#include "MonsterCharacter.generated.h"


class USBStateComponent;
class UMonsterAttributeComponent;
class ATargetPoint;
class USBCombatComponent;
class ASBWeapon;
class URotationComponent;
class UWidgetComponent;
class USphereComponent;
class UProceduralMeshComponent;

UCLASS()
class COPY_STELLARBLADE_API AMonsterCharacter : public ACharacter, public ITargetingInterface, public ISBCombatInterface
{
	GENERATED_BODY()

protected:
	bool bIsDead = false;

	int8 CurrentAttackCount = 0;

	bool bAllowCounterAttack_Blink = false;
	bool bAllowCounterAttack_Repulse = false;

	UPROPERTY(EditAnywhere, Category = "SpecialAttack")
	int8 BlinklAttackCount = 3;

	UPROPERTY(EditAnywhere, Category = "SpecialAttack")
	int8 RepulseAttackCount = 4;

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TargetingSphereComponent;

	UPROPERTY(VisibleAnywhere)
	UMonsterAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	USBStateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere)
	USBCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere)
	URotationComponent* RotationComponent;

	/** LockOn UI Widget */
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* LockOnWidgetComponent;

	/** HealthBar */
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* MonsterStatBarWidget;

	UPROPERTY(EditAnywhere)
	float HealthBarOffsetPosY = 100.f;

protected:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ImpactParticle;

protected:
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	TArray<ATargetPoint*> PatrolPoints;

	UPROPERTY(VisibleAnywhere, Category = "AI | Patrol")
	int32 PatrolIndex = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ASBWeapon>> DefaultWeaponClass;

protected:
	UPROPERTY(EditAnywhere, Category = "Slice")
	FName TargetBoneName = "upperarm_l";

	UPROPERTY(EditAnywhere, Category = "Slice")
	float CreateProceduralMeshDistance = 20.f;

	UPROPERTY(EditAnywhere, Category = "Slice")
	FName ProceduralMeshAttachSocketName = "ProcedrualMesh_Attach";

	UPROPERTY(EditAnywhere, Category = "Slice")
	FName OtherHalfMeshAttachSocketName = "OtherHalfMesh_Attach";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slice")
	UMaterialInterface* CapMaterial;

	int32 NumVertices = 0;
	TMap<int32, int32> VertexIndexMap;
	TArray<FVector> FilteredVerticesArray;
	TArray<int32> Indices;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FColor> Colors;

public:
	AMonsterCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void OnDeath();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
public:
	//TargetingInterface 구현
	virtual void OnTargeted(bool bTargeted) override;
	virtual bool CanBeTargeted() override;

	//CombatInterface 구현.
	virtual void ActivateWeaponCollision(EWeaponType InWeaponType) override;
	virtual void DeactivateWeaponCollision(EWeaponType InWeaponType) override;
	virtual void PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate) override;
	virtual void FinishAttack() override;
	virtual bool IsCombatEnabled() override;

	// 체력바 토글
	void ToggleMonsterStateVisibility(bool bVisibility);

public:
	void SelectVertices(int32 LODIndex);
	void ApplyVertexAlphaToSkeletalMesh();
	void CopySkeletalMeshToProcedural(int32 LODIndex);
	void SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf);

public:
	FORCEINLINE ATargetPoint* GetPatrolPoint()
	{
		return PatrolPoints.Num() >= (PatrolIndex + 1) ? PatrolPoints[PatrolIndex] : nullptr;
	}
	FORCEINLINE void IncrementPatrolIndex()
	{
		PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	}

	ASBWeapon* GetMainWeapon();
	USBCombatComponent* GetCombatComponent();

	void SetCombatEnabled(const bool bEnabled);

	bool IsHaveBlinkAttack() const;

	FORCEINLINE int8 GetCurrentAttackCount() const { return CurrentAttackCount; }
	FORCEINLINE int8 GetBlinkAttackCount() const { return BlinklAttackCount; }
	FORCEINLINE int8 GetRepulseAttackCount() const { return RepulseAttackCount; }

	FORCEINLINE bool GetAllowCounterAttack_Blink() { return bAllowCounterAttack_Blink; }
	FORCEINLINE void SetAllowCounterAttack_Blink(bool isAllow) { bAllowCounterAttack_Blink = isAllow; }

	FORCEINLINE bool GetAllowCounterAttack_Repulse() { return bAllowCounterAttack_Repulse; }
	FORCEINLINE void SetAllowCounterAttack_Repulse(bool isAllow) { bAllowCounterAttack_Repulse = isAllow; }
};

