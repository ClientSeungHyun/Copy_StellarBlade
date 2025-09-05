// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EveCharacter.generated.h"

UCLASS()
class COPY_STELLARBLADE_API AEveCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* LookAction;

public:
	AEveCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void Move(const struct FInputActionValue& Values);
	void Look(const struct FInputActionValue& Values);
};
