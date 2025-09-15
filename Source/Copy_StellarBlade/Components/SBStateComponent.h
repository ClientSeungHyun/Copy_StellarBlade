// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SBStateComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COPY_STELLARBLADE_API USBStateComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/** 이동키 입력 상태 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bMovementInputEnabled = true;

	/** 캐릭터의 현재 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FGameplayTag CurrentState;


public:
	USBStateComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/** 이동 입력이 활성화 상태인지? */
	FORCEINLINE bool MovementInputEnabled() const { return bMovementInputEnabled; };

	/** 이동 입력 토글 */
	void ToggleMovementInput(bool bEnabled, float Duration = 0.f);

protected:
	UFUNCTION()
	void MovementInputEnableAction();

public:
	FORCEINLINE void SetState(const FGameplayTag NewState) { CurrentState = NewState; };
	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentState; };

	/** 현재 상태 초기화 */
	void ClearState();

	/** 현재 State가 TagsToCheck의 어떤것과 같은지 체크 [OR] */
	bool IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const;


};