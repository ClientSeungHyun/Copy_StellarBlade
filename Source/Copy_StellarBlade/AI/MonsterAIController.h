// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

class AMonsterCharacter;
class ASBWeapon;
class AnimInstance;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, bool);

UCLASS()
class COPY_STELLARBLADE_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	FOnTargetChanged OnTargetChange;
	
protected:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTreeAsset;

	/* AI가 주변 환경을 인식할 수 있게 해주는 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* AIPerceptionComponent;

	FTimerHandle TimerHandle;

	UPROPERTY()
	AMonsterCharacter* ControlledEnemy;

	UPROPERTY(VisibleAnywhere);
	bool bHaveTarget;

public:
	AMonsterAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	void UpdateTarget();
	void SetTarget(AActor* NewTarget);
	void HandleBattleStart();
	void PlayBattleStartMontage(AMonsterCharacter* Monster, const ASBWeapon* Weapon, UAnimInstance* AnimInstance);
	void OnBattleStartMontageEnded(UAnimMontage* Montage, bool bInterrupted, AMonsterCharacter* Monster);

	FORCEINLINE bool IsHaveTarget() { return bHaveTarget; }
};
