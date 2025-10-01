// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EveCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SBEveTags.h"
#include "Components/SBEveAtrributeComponent.h"
#include "Components/SBStateComponent.h"
#include "Components/TargetingComponent.h"
#include "Animation/SB_Eve_AnimInstance.h"
#include "Player/SBEveWeapon.h"

AEveCharacter::AEveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 입력에 따른 회전 여부
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 입력 방향을 바라보게 하고 그에 해당하는 회전 속도
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	/** 이동, 감속 속도 */
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->JumpZVelocity = 500;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<USBEveAtrributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<USBStateComponent>(TEXT("State"));
	// LockedOn Targeting
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));
	MovementComp = GetCharacterMovement();

	
}

void AEveCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (SwordClass && !Sword)
	{
		Sword = GetWorld()->SpawnActor<ASBEveWeapon>(
			SwordClass);

		Sword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Sword->SetOwner(this);
	}
}

FVector PreviousRootLocation;
FVector CurrentRootLocation;

void AEveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckLanded();

	/*if(StateComponent->GetCurrentState() != StateComponent->GetPreState())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cur Tag: %s"), *StateComponent->GetCurrentState().ToString());
		UE_LOG(LogTemp, Warning, TEXT("isJump: %d"), isJumping);
	}*/

	//if (!Sword)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Sword Spawn Failed!"));
	
}

void AEveCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AEveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::NewJump);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ThisClass::Running);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ThisClass::StopRunning);

		//어색하면 Cancle사용
		EnhancedInputComponent->BindAction(Normal_AttackAction, ETriggerEvent::Started, this, &ThisClass::NormalAttack);
		EnhancedInputComponent->BindAction(Skill_AttackAction, ETriggerEvent::Started, this, &ThisClass::SkillAttack);
		
		// LockedOn
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &ThisClass::LockOnTarget);
		/*EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this, &ThisClass::LeftTarget);
		EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &ThisClass::RightTarget);*/
	}

}

bool AEveCharacter::IsMoving() const
{
	if (MovementComp)
	{
		return MovementComp->Velocity.Size2D() > 3.f && MovementComp->GetCurrentAcceleration() != FVector::Zero();
	}

	return false;
}

void AEveCharacter::Move(const FInputActionValue& Values)
{
	FVector2D MovementVector = Values.Get<FVector2D>();

	if (isJumping == false)
	{
		StateComponent->SetState(SBEveTags::Eve_State_Walking);
	}

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotator(0, Rotation.Yaw, 0);

		const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void AEveCharacter::Look(const FInputActionValue& Values)
{
	FVector2D LookDirection = Values.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookDirection.X);
		AddControllerPitchInput(LookDirection.Y);
	}
}

void AEveCharacter::Running()
{
	if (IsMoving())
	{
		if (StateComponent->GetCurrentState() == SBEveTags::Eve_State_Walking)
		{
			StateComponent->SetState(SBEveTags::Eve_State_Running);
		}

		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		StopRunning();
	}
}

void AEveCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void AEveCharacter::Idle()
{
	if (StateComponent->GetCurrentState() != SBEveTags::Eve_State_Idle)
	{
		StateComponent->SetState(SBEveTags::Eve_State_Idle);
	}
}

void AEveCharacter::NewJump()
{
	if(isJumping == false && isAttacking == false)
	{
		isJumping = true;
		StateComponent->SetState(SBEveTags::Eve_State_JumpStart);
		Jump();
	}
}

void AEveCharacter::CheckLanded()
{
	bool bIsFalling = MovementComp->IsFalling();

	if (bIsFalling)
	{
		StateComponent->SetState(SBEveTags::Eve_State_Falling);
	}

	if (isJumping && bIsFalling == false)
	{
		isJumping = false;
		Idle();
	}

}

void AEveCharacter::LockOnTarget()
{
	TargetingComponent->ToggleLockOn();
}

void AEveCharacter::NormalAttack()
{
	if (CanPerformAttack() == false)
		return;

	ExecuteComboAttack(SBEveTags::Eve_Attack_NormalAttack);
}

void AEveCharacter::SkillAttack()
{
	if (CanPerformAttack() == false)
		return;

	ExecuteComboAttack(SBEveTags::Eve_Attack_SkillAttack);
}

void AEveCharacter::EnableComboWindow()
{
	bCanComboInput = true;
	UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

void AEveCharacter::DisableComboWindow()
{
	bCanComboInput = false;

	if (bSavedComboInput)
	{
		bSavedComboInput = false;
		ComboCounter++;
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next combo = %d"), ComboCounter);
		DoAttack(lastAttackTag);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input received"));
	}
}

bool AEveCharacter::CanPerformAttack()
{
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(SBEveTags::Eve_State_Falling);
	CheckTags.AddTag(SBEveTags::Eve_State_JumpStart);

	if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false)
		return true;
	else
		return false;
}

void AEveCharacter::ResetCombo()
{
	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

void AEveCharacter::DoAttack(const FGameplayTag& AttackTypeTag)
{
	check(StateComponent)
	check(AttributeComponent)

	lastAttackTag = AttackTypeTag;
	StateComponent->ToggleMovementInput(false);
	isAttacking = true;

	UAnimMontage* Montage = Sword->GetMontageForTag(AttackTypeTag, ComboCounter);
	if (!Montage)
	{
		// 콤보 한계 도달.
		ComboCounter = 0;
		Montage = Sword->GetMontageForTag(AttackTypeTag, ComboCounter);
	}

	PlayAnimMontage(Montage);
}

void AEveCharacter::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	static int32 test = 0;
	//StateComponent->GetCurrentState() != SBEveTags::Eve_State_Attacking
	if (isAttacking == false)
	{
		test++;
		StateComponent->SetState(SBEveTags::Eve_State_Attacking);
		UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
		ResetCombo();
		bComboSequenceRunning = true;

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}

	if (bCanComboInput)
	{
		UE_LOG(LogTemp, Warning, TEXT("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"));
		// 콤보 윈도우가 열려 있을 때 - 최적의 타이밍
		bSavedComboInput = true;
	}
}

void AEveCharacter::AttackFinished(const float ComboResetDelay)
{
	UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true);
		isAttacking = false;
	}
	// ComboResetDelay 후에 콤보 시퀀스 종료
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
}

//void AEveCharacter::LeftTarget()
//{
//	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Left);
//}
//
//void AEveCharacter::RightTarget()
//{
//	TargetingComponent->SwitchingLockedOnActor(ESwitchingDirection::Right);
//}

