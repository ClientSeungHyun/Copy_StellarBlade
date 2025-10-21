// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
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
	class UInputAction* MoveAction_F;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction_B;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction_L;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction_R;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunDodgeAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Normal_AttackAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Skill_AttackAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Guard_Action;

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

	/** 걷는 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float SlowSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASBEveWeapon> SwordClass;

	UPROPERTY()
	ASBEveWeapon* Sword;

	UPROPERTY(EditAnywhere, Category = "Run/Dodge")
	float DodgeThreshold = 0.25f; // 0.25초 이하로 누르면 회피

protected:
	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimFront;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimBack;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimLeft;

	UPROPERTY(EditAnywhere, Category = "Montage | HitReact")
	UAnimMontage* HitReactAnimRight;

	UPROPERTY(EditAnywhere, Category = "Montage | Dodge")
	UAnimMontage* DodgeAnimFront;

	UPROPERTY(EditAnywhere, Category = "Montage | Dodge")
	UAnimMontage* DodgeAnimBack;

	UPROPERTY(EditAnywhere, Category = "Montage | Dodge")
	UAnimMontage* DodgeAnimLeft;

	UPROPERTY(EditAnywhere, Category = "Montage | Dodge")
	UAnimMontage* DodgeAnimRight;

protected: //Combo System
	//콤보 작동 중인지
	bool bComboSequenceRunning = false;

	//콤보 입력 가능한지
	bool bCanComboInput = false;

	//콤보 카운터
	int32 ComboCounter = 0;

	//콤보 입력 여부
	bool bSavedComboInput = false;

	//콤보 리셋 타이머 핸들
	FTimerHandle ComboResetTimerHandle;

private:
	bool isJumping = false;
	bool isAttacking = false;
	bool isGuarding = false;
	bool isPerfectGuarded = false;

	bool isPressed_W = false;
	bool isPressed_A = false;
	bool isPressed_S = false;
	bool isPressed_D = false;
	bool bPressShift = false;

	bool isLockOn = false;

	float PressShiftTime = 0.0f;
	float ShiftPressedTime = 0.0f;
	float GuardStartTime = 0.0f;
	float BlinkMoveBackDistance = 10.0f;

	FGameplayTag lastAttackTag;
	UAnimMontage* CurrentPlaying_AM = nullptr;

public:
	AEveCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventController, AActor* DamageCauser) override;

	virtual void OnDeath();

public:
	FORCEINLINE USBStateComponent* GetStateComponent() const { return StateComponent; }
	USBEveAtrributeComponent* GetAttributeComponent()  { return AttributeComponent; }

	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);

	bool GetIsGuarding() { return isGuarding; }
	bool GetIsPerfectGuarded() { return isPerfectGuarded; }

	bool GetPressed_W() { return isPressed_W; }
	bool GetPressed_A() { return isPressed_A; }
	bool GetPressed_S() { return isPressed_S; }
	bool GetPressed_D() { return isPressed_D; }

	bool IsLockOn() { return isLockOn; }

	ASBEveWeapon* GetWeapon() { return Sword; }

	UCharacterMovementComponent* MovementComp = nullptr;

protected:
	/** 캐릭터가 이동중인지 체크 */
	bool IsMoving() const;

protected:
	void Move(const struct FInputActionValue& Values);
	void Look(const struct FInputActionValue& Values);

	void Pressed_W(const struct FInputActionValue& Values);
	void Pressed_A(const struct FInputActionValue& Values);
	void Pressed_S(const struct FInputActionValue& Values);
	void Pressed_D(const struct FInputActionValue& Values);
	void Unpress_W();
	void Unpress_A();
	void Unpress_S();
	void Unpress_D();

	void Running();
	void StopRunning();
	void Pressed_Shift();
	void Dodge();

	void Idle();
	void NewJump();
	void StartGuard();
	void EndGuard();
	void PerfectGuard();

	void CheckLanded();

	/** LockedOn */
	void LockOnTarget();
	//void LeftTarget();
	//void RightTarget();

	void NormalAttack();
	void SkillAttack();
	void BlinkAttack();
	void RepulseAttack();

	bool CanPerformAttack();
	void ResetCombo();
	void DoAttack(const FGameplayTag& AttackTypeTag);
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);

	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;

	void TeleportBehindTarget(AActor* TargetActor, float DistanceBehind);

};
