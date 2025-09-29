// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SB_Eve_AnimInstance.generated.h"

class UCharacterMovementComponent;
class USBStateComponent;
class AEveCharacter;

UCLASS()
class COPY_STELLARBLADE_API USB_Eve_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AEveCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UCharacterMovementComponent* MovementComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	bool bIsJumpingStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Data")
	float Direction;

private:
	USBStateComponent* PlayerStateComp = nullptr;
public:
	USB_Eve_AnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
