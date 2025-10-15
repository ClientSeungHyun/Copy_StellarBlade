// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MonsterAttributeComponent.h"

#include "SBGameplayTags.h"
#include "SBStateComponent.h"

UMonsterAttributeComponent::UMonsterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UMonsterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();


}


void UMonsterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMonsterAttributeComponent::ToggleStaminaRegeneration(bool bEnabled, float StartDelay)
{
	//if (bEnabled)
	//{
	//	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle) == false)
	//	{
	//		GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ThisClass::RegenerateStaminaHandler, 0.1f, true, StartDelay);
	//	}
	//}
	//else
	//{
	//	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	//}
}

void UMonsterAttributeComponent::BroadcastAttributeChanged(EAttributeType InAttributeType) const
{
	if (OnAttributeChanged.IsBound())
	{
		// Type에 따라서 값을 골라준다.
		float Value = 0.f;
		switch (InAttributeType)
		{
		case EAttributeType::Health:
			Value = GetHealthRatio();
			break;

		case EAttributeType::Shield:
			Value = GetBaseShieldGage();
			break;

		//case EAttributeType::Stamina:
		//	Value = GetStaminaRatio();
		//	break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Value);
	}
}

void UMonsterAttributeComponent::TakeDamageAmount(float DamageAmount)
{
	if (CheckEnoughShieldGage())
	{
		DamageAmount *= 0.3f;
		DecreaseShieldGage();
	}

	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth);

	BroadcastAttributeChanged(EAttributeType::Health);

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

bool UMonsterAttributeComponent::CheckEnoughShieldGage()
{
	if (BaseShieldGage > 0)
		return true;

	return false;
}

bool UMonsterAttributeComponent::CheckEnoughStamina()
{
	if (BaseStamina > 0)
		return true;

	return false;
}

void UMonsterAttributeComponent::DecreaseShieldGage()
{
	if (CheckEnoughShieldGage())
		--BaseShieldGage;

	BroadcastAttributeChanged(EAttributeType::Shield);
}

void UMonsterAttributeComponent::DecreaseStamina()
{
	if (CheckEnoughStamina())
		--BaseStamina;

	BroadcastAttributeChanged(EAttributeType::Stamina);
}

