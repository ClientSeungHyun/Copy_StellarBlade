// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBStateComponent.h"
#include "Kismet/KismetSystemLibrary.h"

USBStateComponent::USBStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void USBStateComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USBStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USBStateComponent::ToggleMovementInput(bool bEnabled, float Duration)
{
	if (bEnabled)
	{
		FLatentActionInfo LatentAction;
		LatentAction.CallbackTarget = this;
		LatentAction.ExecutionFunction = "MovementInputEnableAction";
		LatentAction.Linkage = 0;
		LatentAction.UUID = 0;

		UKismetSystemLibrary::RetriggerableDelay(GetWorld(), Duration, LatentAction);
	}
	else
	{
		bMovementInputEnabled = false;
	}
}

void USBStateComponent::MovementInputEnableAction()
{
	bMovementInputEnabled = true;
	ClearState();
}

void USBStateComponent::ClearState()
{
	CurrentState = FGameplayTag::EmptyTag;
}

bool USBStateComponent::IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const
{
	return TagsToCheck.HasTagExact(CurrentState);
}
