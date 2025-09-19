// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBEveAtrributeComponent.h"

USBEveAtrributeComponent::USBEveAtrributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void USBEveAtrributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void USBEveAtrributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

