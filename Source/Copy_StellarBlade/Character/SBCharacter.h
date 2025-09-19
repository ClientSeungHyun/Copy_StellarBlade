// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SBCharacter.generated.h"

class USBStateComponent;
class USBEveAtrributeComponent;
class USB_Eve_AnimInstance;

UCLASS()
class COPY_STELLARBLADE_API ASBCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

// Input Section
private:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* RunAction;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BodyMesh;

	/** 캐릭터의 각종 스탯 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USBEveAtrributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USBStateComponent* StateComponent;

protected:
	/** 질주 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float SprintingSpeed = 750.f;

	/** 일반 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float NormalSpeed = 500.f;

public:
	ASBCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE USBStateComponent* GetStateComponent() const { return StateComponent; };

protected:
	/** 캐릭터가 이동중인지 체크 */
	bool IsMoving() const;

protected:
	void Move(const struct FInputActionValue& Values);
	void Look(const struct FInputActionValue& Values);
	/** 질주 */
	void Running();
	/** 질주 중단 */
	void StopRunning();
	void Idle();

};
