// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Animation/AnimInstance.h"
#include "Monster_AnimInstance.generated.h"

class UCharacterMovementComponent;
class AMonsterAIController;

UCLASS()
class COPY_STELLARBLADE_API UMonster_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ACharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AMonsterAIController* MonsterAIController;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsHarassing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	FVector Direction;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	//FVector HarassDirection;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Data")
	bool bCombatEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Data")
	ECombatType CombatType = ECombatType::None;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Data")
	bool bHaveTarget = false;

public:
	UMonster_AnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION()
	void AnimNotify_ResetMovementInput();

	UFUNCTION()
	void AnimNotify_ResetState();

	// Animation
public:
	void UpdateCombatMode(const ECombatType InCombatType);

public:
	void BindAIController(AController* NewController);

public:
	FORCEINLINE bool IsHarassing() { return bIsHarassing; }
	FORCEINLINE void SetIsHarassing(bool InIsHarassing) { bIsHarassing = InIsHarassing; }

	// Delegate functions
protected:
	void OnChangedCombat(const bool bInCombatEnabled);
	void OnChangedTarget(const bool bInHaveTarget);


};
