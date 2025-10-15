// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Components/ActorComponent.h"
#include "MonsterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EAttributeType, float);
DECLARE_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COPY_STELLARBLADE_API UMonsterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/** 스탯 변경 Delegate */
	FDelegateOnAttributeChanged OnAttributeChanged;

	/** 죽음을 알리는 Delegate */
	FOnDeath OnDeath;

protected:
	UPROPERTY(EditAnywhere, Category = "Health")
	float BaseHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 100.f;


	UPROPERTY(EditAnywhere, Category = "Shield")
	float BaseShieldGage = 6.f;

	UPROPERTY(EditAnywhere, Category = "Shield")
	float MaxShieldGage = 6.f;


	UPROPERTY(EditAnywhere, Category = "Stamina")
	float BaseStamina = 4.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 4.f;


	/** 스태미나 재충전 타이머 핸들 */
	FTimerHandle StaminaRegenTimerHandle;

public:
	UMonsterAttributeComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE float GetBaseHealth() const { return BaseHealth; };
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; };

	FORCEINLINE float GetBaseShieldGage() const { return BaseShieldGage; };
	FORCEINLINE float GetMaxShieldGage() const { return MaxShieldGage; };

	FORCEINLINE float GetBaseStamina() const { return BaseStamina; };
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; };

	FORCEINLINE float GetHealthRatio() const { return BaseHealth / MaxHealth; };

	/** 스테미너 재충전/중지 토글 */
	void ToggleStaminaRegeneration(bool bEnabled, float StartDelay = 2.f);

	/** 스텟 변경을 통지하는 Broadcast Function */
	void BroadcastAttributeChanged(EAttributeType InAttributeType) const;

	void TakeDamageAmount(float DamageAmount);

private:
	bool CheckEnoughShieldGage();
	bool CheckEnoughStamina();

	void DecreaseShieldGage();
	void DecreaseStamina();
};
