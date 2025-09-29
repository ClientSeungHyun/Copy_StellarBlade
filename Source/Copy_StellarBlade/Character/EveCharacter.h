// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EveCharacter.generated.h"

class USBStateComponent;
class USBEveAtrributeComponent;
class USB_Eve_AnimInstance;
class ASBEveWeapon;
class UTargetingComponent;

UCLASS()
class COPY_STELLARBLADE_API AEveCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


// Input Section
private:
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** LockedOn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnTargetAction;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BodyMesh;

	/** 캐릭터의 각종 스탯 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USBEveAtrributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USBStateComponent* StateComponent;

	/** LockedOn Targeting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTargetingComponent* TargetingComponent;

protected:
	/** 질주 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float SprintingSpeed = 600.f;

	/** 일반 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float NormalSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASBEveWeapon> SwordClass;

	UPROPERTY()
	ASBEveWeapon* Sword;

private:
	bool isJumping = false;

public:
	AEveCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE USBStateComponent* GetStateComponent() const { return StateComponent; };

	UCharacterMovementComponent* MovementComp = nullptr;

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
	void NewJump();
	void CheckLanded();

	/** LockedOn */
	void LockOnTarget();
	//void LeftTarget();
	//void RightTarget();
};
