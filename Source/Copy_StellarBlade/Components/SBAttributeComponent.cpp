// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBAttributeComponent.h"

#include "SBGameplayTags.h"
#include "SBStateComponent.h"

USBAttributeComponent::USBAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void USBAttributeComponent::BeginPlay()
{
	Super::BeginPlay();


}


void USBAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool USBAttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return BaseStamina >= StaminaCost;
}

void USBAttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.f, MaxStamina);

	BroadcastAttributeChanged(ESBAttributeType::Stamina);
}

void USBAttributeComponent::ToggleStaminaRegeneration(bool bEnabled, float StartDelay)
{
	if (bEnabled)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ThisClass::RegenerateStaminaHandler, 0.1f, true, StartDelay);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void USBAttributeComponent::BroadcastAttributeChanged(ESBAttributeType InAttributeType) const
{
	if (OnAttributeChanged.IsBound())
	{
		// Type에 따라서 값을 골라준다.
		float Ratio = 0.f;
		switch (InAttributeType)
		{
		case ESBAttributeType::Stamina:
			Ratio = GetStaminaRatio();
			break;

		case ESBAttributeType::Health:
			break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Ratio);
	}
}

void USBAttributeComponent::TakeDamageAmount(float DamageAmount)
{
	// 체력 차감.
	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth);

	BroadcastAttributeChanged(ESBAttributeType::Health);

	if (BaseHealth <= 0.f)
	{
		// Call Death Delegate
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}

		// Set Death State
		if (USBStateComponent* StateComp = GetOwner()->FindComponentByClass<USBStateComponent>())
		{
			StateComp->SetState(SBGameplayTags::Character_State_Death);
		}
	}
}

void USBAttributeComponent::RegenerateStaminaHandler()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.f, MaxStamina);

	BroadcastAttributeChanged(ESBAttributeType::Stamina);

	if (BaseStamina >= MaxStamina)
	{
		ToggleStaminaRegeneration(false);
	}
}

