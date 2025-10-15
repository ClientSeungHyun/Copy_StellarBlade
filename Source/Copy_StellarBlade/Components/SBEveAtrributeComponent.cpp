// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBEveAtrributeComponent.h"
#include "SBStateComponent.h"
#include "SBEveTags.h"
#include "Character/EveCharacter.h"

USBEveAtrributeComponent::USBEveAtrributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void USBEveAtrributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AEveCharacter>(GetOwner());
}


void USBEveAtrributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USBEveAtrributeComponent::BroadcastAttributeChanged(EAttributeType InAttributeType) const
{
	if (OnAttributeChanged.IsBound())
	{
		// Type에 따라서 값을 골라준다.
		float Ratio = 0.f;
		switch (InAttributeType)
		{
		case EAttributeType::Health:
			Ratio = GetHealthRatio();
			break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Ratio);
	}
}

void USBEveAtrributeComponent::TakeDamageAmount(float DamageAmount)
{
	// 체력 차감.
	if (OwnerCharacter != nullptr && OwnerCharacter->GetIsGuarding())
	{
		DamageAmount = DamageAmount * 0.7f;
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
			StateComp->SetState(SBEveTags::Eve_State_Death);
		}
	}
}

