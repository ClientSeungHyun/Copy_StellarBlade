// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SBInteract.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USBInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COPY_STELLARBLADE_API ISBInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact(AActor* InteractionActor) = 0;
};
