// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EveCharacter.h"
#include "SBDefine.h"
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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DamageEvents.h"
#include "Monster/MonsterCharacter.h"
#include "UI/PlayerUI/EveHUD.h"

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
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->JumpZVelocity = 500;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 360.0f;
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

	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
}

void AEveCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (EveHUDClass)
	{
		Eve_HUD = CreateWidget<UEveHUD>(GetWorld(), EveHUDClass);
		if (Eve_HUD)
		{
			Eve_HUD->AddToViewport();
		}
	}

	if (SwordClass && !Sword)
	{
		Sword = GetWorld()->SpawnActor<ASBEveWeapon>(
			SwordClass);

		Sword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Sword->SetOwner(this);
		Sword->EquipItem();
	}


	FreePlayerMovementAtLockon_CheckTags.AddTag(SBEveTags::Eve_State_Dodge);
	FreePlayerMovementAtLockon_CheckTags.AddTag(SBEveTags::Eve_State_PerfectDodge);
	FreePlayerMovementAtLockon_CheckTags.AddTag(SBEveTags::Eve_State_Running);
}

FVector PreviousRootLocation;
FVector CurrentRootLocation;

void AEveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckLanded();

	isLockOn = TargetingComponent->IsLockOn();


	if (isLockOn)
	{
		if (StateComponent->IsCurrentStateEqualToAny(FreePlayerMovementAtLockon_CheckTags))
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		else
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}

	if (isPressShift)
	{
		float CurrentPressTime = FMath::Clamp(GetWorld()->GetTimeSeconds() - PressShiftTime,0.0f,1.0f);
		//UE_LOG(LogTemp, Warning, TEXT("Press Time : %.2f"), CurrentPressTime);

		if (CurrentPressTime < 0.2f)
		{
			bCanPerfectDodge = true;
		}

		if (CurrentPressTime < 0.5f && StateComponent->GetCurrentState() != SBEveTags::Eve_State_Dodge && StateComponent->GetCurrentState() != SBEveTags::Eve_State_PerfectDodge)
		{
			bCanPerfectDodge = false;
			Dodge();
		}
		else if(CurrentPressTime > 0.5f && StateComponent->GetCurrentState() != SBEveTags::Eve_State_Running)
		{
			Running();
		}
	}

	//if(StateComponent->GetCurrentState() != StateComponent->GetPreState())
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Cur Tag: %s"), *StateComponent->GetCurrentState().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("isJump: %d"), isJumping);
	//}	
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

		EnhancedInputComponent->BindAction(Guard_Action, ETriggerEvent::Started, this, &ThisClass::StartGuard);
		EnhancedInputComponent->BindAction(Guard_Action, ETriggerEvent::Completed, this, &ThisClass::EndGuard);

		EnhancedInputComponent->BindAction(RunDodgeAction, ETriggerEvent::Started, this, &ThisClass::Pressed_Shift);
		EnhancedInputComponent->BindAction(RunDodgeAction, ETriggerEvent::Completed, this, &ThisClass::StopRunning);

		EnhancedInputComponent->BindAction(MoveAction_F, ETriggerEvent::Triggered, this, &ThisClass::Pressed_W);
		EnhancedInputComponent->BindAction(MoveAction_B, ETriggerEvent::Triggered, this, &ThisClass::Pressed_S);
		EnhancedInputComponent->BindAction(MoveAction_L, ETriggerEvent::Triggered, this, &ThisClass::Pressed_A);
		EnhancedInputComponent->BindAction(MoveAction_R, ETriggerEvent::Triggered, this, &ThisClass::Pressed_D);
		EnhancedInputComponent->BindAction(MoveAction_F, ETriggerEvent::Completed, this, &ThisClass::Unpress_W);
		EnhancedInputComponent->BindAction(MoveAction_B, ETriggerEvent::Completed, this, &ThisClass::Unpress_S);
		EnhancedInputComponent->BindAction(MoveAction_L, ETriggerEvent::Completed, this, &ThisClass::Unpress_A);
		EnhancedInputComponent->BindAction(MoveAction_R, ETriggerEvent::Completed, this, &ThisClass::Unpress_D);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EnhancedInputComponent->BindAction(Normal_AttackAction, ETriggerEvent::Started, this, &ThisClass::NormalAttack);
		EnhancedInputComponent->BindAction(Skill_AttackAction, ETriggerEvent::Started, this, &ThisClass::SkillAttack);
		EnhancedInputComponent->BindAction(Use_Potion_Action, ETriggerEvent::Started, this, &ThisClass::UsePotion);
		
		// LockedOn
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &ThisClass::LockOnTarget);
		/*EnhancedInputComponent->BindAction(LeftTargetAction, ETriggerEvent::Started, this, &ThisClass::LeftTarget);
		EnhancedInputComponent->BindAction(RightTargetAction, ETriggerEvent::Started, this, &ThisClass::RightTarget);*/
	}

}

float AEveCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float HitTime = GetWorld()->GetTimeSeconds();

	if (StateComponent->GetCurrentState() == SBEveTags::Eve_State_PerfectDodge)
		return 0.f;

	if (bCanPerfectDodge && HitTime - DodgeStartTime <= PerfectDodgeTime)
	{
		PerfectDodge();
		return 0.f;
	}

	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
		PlayShakeCamera();
		HitReaction(this);
		GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Damaged : %f"), ActualDamage));
	}

	//if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	//{
	//	const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

	//	// 데미지 방향
	//	FVector ShotDirection = PointDamageEvent->ShotDirection;
	//	// 히트 위치 (표면 접촉 관점)
	//	FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
	//	// 히트 방향
	//	FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
	//	// 히트한 객체의 Location (객체 중심 관점)
	//	FVector HitLocation = PointDamageEvent->HitInfo.Location;

	//}

	return ActualDamage;
}

void AEveCharacter::OnDeath()
{
	//if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	//{
	//	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}

	//// Ragdoll
	//if (USkeletalMeshComponent* MeshComp = GetMesh())
	//{
	//	MeshComp->SetCollisionProfileName("Ragdoll");
	//	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//	MeshComp->SetSimulatePhysics(true);
	//}
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
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(SBEveTags::Eve_State_Dodge);
	CheckTags.AddTag(SBEveTags::Eve_State_PerfectDodge);
	CheckTags.AddTag(SBEveTags::Eve_State_Attacking);

	if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false && isJumping == false)
	{
		FVector2D MovementVector = Values.Get<FVector2D>();

		if(isPressShift == false)
			StateComponent->SetState(SBEveTags::Eve_State_Walking);

		if (isGuarding)
			GetCharacterMovement()->MaxWalkSpeed = SlowSpeed;
		else if(StateComponent->GetCurrentState() == SBEveTags::Eve_State_Running)
			GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

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

void AEveCharacter::Pressed_W(const FInputActionValue& Values)
{
	isPressed_W = true;

	Move(Values);
}

void AEveCharacter::Pressed_A(const FInputActionValue& Values)
{
	isPressed_A = true;

	Move(Values);
}

void AEveCharacter::Pressed_S(const FInputActionValue& Values)
{
	isPressed_S = true;

	Move(Values);
}

void AEveCharacter::Pressed_D(const FInputActionValue& Values)
{
	isPressed_D = true;

	Move(Values);
}

void AEveCharacter::Unpress_W()
{
	isPressed_W = false;
}

void AEveCharacter::Unpress_A()
{
	isPressed_A = false;
}

void AEveCharacter::Unpress_S()
{
	isPressed_S = false;
}

void AEveCharacter::Unpress_D()
{
	isPressed_D = false;
}

void AEveCharacter::Running()
{
	//닷지 후 달리기 시 몽타주가 완전히 끝나지 않아도 상태 전환
	if (StateComponent->GetCurrentState() == SBEveTags::Eve_State_Dodge)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		float Position = AnimInstance->Montage_GetPosition(CurrentPlaying_AM);
		float Length = CurrentPlaying_AM->GetPlayLength();
		float Ratio = Position / Length;

		if (Ratio > 0.5f)
		{
			AnimInstance->Montage_Stop(0.2f, CurrentPlaying_AM);
			StateComponent->SetState(SBEveTags::Eve_State_Running);
		}
	}
	else	
	{
		StateComponent->SetState(SBEveTags::Eve_State_Running);
	}
}

void AEveCharacter::StopRunning()
{
	isPressShift = false;
}

void AEveCharacter::Dodge()
{
	StateComponent->SetState(SBEveTags::Eve_State_Dodge);

	DodgeStartTime = GetWorld()->GetTimeSeconds();

	if (TargetingComponent->IsLockOn() == false)
	{
		if (isPressed_W)
		{
			PlayAnimMontage(DodgeAnimFront);
			CurrentPlaying_AM = DodgeAnimFront;
		}
		else
		{
			PlayAnimMontage(DodgeAnimBack);
			CurrentPlaying_AM = DodgeAnimBack;
		}
	}
	else
	{
		if (isPressed_A)
		{
			PlayAnimMontage(DodgeAnimLeft);
			CurrentPlaying_AM = DodgeAnimLeft;
		}
		else if (isPressed_D)
		{
			PlayAnimMontage(DodgeAnimRight);
			CurrentPlaying_AM = DodgeAnimRight;
		}
		else if (isPressed_W)
		{
			PlayAnimMontage(DodgeAnimFront);
			CurrentPlaying_AM = DodgeAnimFront;
		}
		else
		{
			PlayAnimMontage(DodgeAnimBack);
			CurrentPlaying_AM = DodgeAnimBack;
		}
	}
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

void AEveCharacter::StartGuard()
{
	isGuarding = true;
	GuardStartTime = GetWorld()->GetTimeSeconds();
	isPerfectGuarded = false;
}

void AEveCharacter::EndGuard()
{
	isGuarding = false;
}

void AEveCharacter::PerfectGuard()
{
	PlayWorldSlowMotion(0.3,0.1);
	isPerfectGuarded = true;
	AttributeComponent->AddBetaEnergy(10.f);
}

void AEveCharacter::PerfectDodge()
{
	StateComponent->SetState(SBEveTags::Eve_State_PerfectDodge);

	PlayWorldSlowMotion(0.8, 1.0);

	if (TargetingComponent->IsLockOn())
	{
		if (isPressed_A)
		{
			PlayAnimMontage(PerfectDodgeAnim_Left);
			CurrentPlaying_AM = PerfectDodgeAnim_Left;
		}
		else if (isPressed_D)
		{
			PlayAnimMontage(PerfectDodgeAnim_Right);
			CurrentPlaying_AM = PerfectDodgeAnim_Right;
		}
		else
		{
			PlayAnimMontage(PerfectDodgeAnim_Back);
			CurrentPlaying_AM = PerfectDodgeAnim_Back;
		}
	}
	else
	{
		PlayAnimMontage(PerfectDodgeAnim_Back);
		CurrentPlaying_AM = PerfectDodgeAnim_Back;
	}
}

void AEveCharacter::UsePotion()
{
	if (AttributeComponent->GetHealthRatio() >= 1.f || AttributeComponent->GetCurrentPotionCount() == 0)
		return;

	PlayAnimMontage(PotionAnim);
	bUsePotion = true;
	AttributeComponent->UsePotion();
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
	GetCharacterMovement()->bOrientRotationToMovement = !TargetingComponent->IsLockOn();
}

void AEveCharacter::NormalAttack()
{
	if (CanPerformAttack() == false)
		return;

	ExecuteComboAttack(SBEveTags::Eve_Attack_NormalAttack);
}

void AEveCharacter::SkillAttack()
{
	if (CanPerformAttack() == false || AttributeComponent->GetBetaEnergy() < 40.f)
		return;

	AttributeComponent->DecreaseBetaEnergy();
	ExecuteComboAttack(SBEveTags::Eve_Attack_SkillAttack);
}

void AEveCharacter::BlinkAttack()
{
	if (CanPerformAttack() == false)
		return;

	ExecuteComboAttack(SBEveTags::Eve_Attack_BlinkAttack);
}

void AEveCharacter::RepulseAttack()
{
	ExecuteComboAttack(SBEveTags::Eve_Attack_RepulseAttack);
}

void AEveCharacter::EnableComboWindow()
{
	bCanComboInput = true;
	//UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

void AEveCharacter::DisableComboWindow()
{
	bCanComboInput = false;

	if (bSavedComboInput)
	{
		bSavedComboInput = false;
		ComboCounter++;
		//UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next combo = %d"), ComboCounter);
		DoAttack(Sword->GetLastAttackTag());
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input received"));
	}
}

bool AEveCharacter::CanPerformAttack()
{
	check(StateComponent);

	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(SBEveTags::Eve_State_Falling);
	CheckTags.AddTag(SBEveTags::Eve_State_JumpStart);
	CheckTags.AddTag(SBEveTags::Eve_State_Hit);
	CheckTags.AddTag(SBEveTags::Eve_State_Dodge);

	if (StateComponent->IsCurrentStateEqualToAny(CheckTags) == false && isGuarding == false)
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

	Sword->SetLastAttackTag(AttackTypeTag);
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
	if (isAttacking == false)
	{
		StateComponent->SetState(SBEveTags::Eve_State_Attacking);
		//UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
		ResetCombo();
		bComboSequenceRunning = true;

		DoAttack(AttackTypeTag);
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}

	if (bCanComboInput)
	{
		//UE_LOG(LogTemp, Warning, TEXT("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"));
		// 콤보 윈도우가 열려 있을 때 - 최적의 타이밍
		bSavedComboInput = true;
	}
}

void AEveCharacter::PlayShakeCamera()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	PC->PlayerCameraManager->StartCameraShake(ShakeCamera, 1.0f);
}

void AEveCharacter::ApplyHitLag(float HitLagDuration, float HitLagScale)
{
	// 전체 게임 시간 멈칫
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	// 0.05초 후 원래 속도로 복귀
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,
		[this]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		},
		0.05f,
		false 
	);
}

void AEveCharacter::PlayWorldSlowMotion(float SlowAmount, float Duration)
{ // 슬로우 강도 보정 (0 ~ 1)
	SlowAmount = FMath::Clamp(SlowAmount, 0.01f, 1.0f);

	UE_LOG(LogTemp, Warning, TEXT("SlowMotion: Amount=%.2f, Duration=%.2f"), SlowAmount, Duration);

	// 월드 슬로우 적용
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowAmount);

	// Duration 후 원상복구
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		},
		Duration,
		false
	);
}

void AEveCharacter::HitReaction(const AActor* Attacker)
{
	float HitTime = GetWorld()->GetTimeSeconds();

	//퍼팩트 패링(가드)
	if (isGuarding && HitTime - GuardStartTime <= PerfectGuardTime)
	{
		PerfectGuard();
		return;
	}
	else if(isGuarding)
	{
		return;
	}

	if (UAnimMontage* HitReactAnimMontage = GetHitReactAnimation(Attacker))
	{
		StateComponent->SetState(SBEveTags::Eve_State_Hit);

		float DelaySeconds = PlayAnimMontage(HitReactAnimMontage);

		if (isAttacking)
			isAttacking = false;
	}
}

UAnimMontage* AEveCharacter::GetHitReactAnimation(const AActor* Attacker) const
{
	// LookAt 회전값을 구합니다. (현재 Actor가 공격자를 바라보는 회전값)
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation());
	// 현재 Actor의 회전값과 LookAt 회전값의 차이를 구합니다.
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), LookAtRotation);
	// Z축 기준의 회전값 차이만을 취합니다.
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f))
	{
		HitDirection = EHitDirection::Front;
		//UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f))
	{
		HitDirection = EHitDirection::Left;
		//UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f))
	{
		HitDirection = EHitDirection::Back;
		//UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f))
	{
		HitDirection = EHitDirection::Right;
		//UE_LOG(LogTemp, Log, TEXT("Right"));
	}

	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection)
	{
	case EHitDirection::Front:
		SelectedMontage = HitReactAnimFront;
		break;
	case EHitDirection::Back:
		SelectedMontage = HitReactAnimBack;
		break;
	case EHitDirection::Left:
		SelectedMontage = HitReactAnimLeft;
		break;
	case EHitDirection::Right:
		SelectedMontage = HitReactAnimRight;
		break;
	}

	return SelectedMontage;
}

void AEveCharacter::TeleportBehindTarget(AActor* TargetActor, float DistanceBehind)
{
	if (!TargetActor) return;

	// 타깃의 위치와 방향 가져오기
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator TargetRotation = TargetActor->GetActorRotation();
	FVector TargetForward = TargetActor->GetActorForwardVector();

	// 타깃 뒤쪽 위치 계산
	FVector TeleportLocation = TargetLocation - (TargetForward * DistanceBehind);

	// 현재 캐릭터 높이 보정 (지면보다 아래로 순간이동 방지)
	TeleportLocation.Z = GetActorLocation().Z;

	// 회전 방향: 타깃을 바라보게 설정 (선택 사항)
	FRotator NewRotation = (TargetLocation - TeleportLocation).Rotation();

	// 순간이동 실행
	SetActorLocationAndRotation(TeleportLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);

	//UE_LOG(LogTemp, Warning, TEXT("상대 뒤 %.1fcm 위치로 순간이동!"), DistanceBehind);
}

void AEveCharacter::Pressed_Shift()
{
	if (isLockOn)
	{
		AActor* targetActor = TargetingComponent->GetLockedTargetActor();
		AMonsterCharacter* targetMonster = Cast<AMonsterCharacter>(targetActor);

		if (targetMonster->GetAllowCounterAttack_Blink())// && isPressed_W)
		{
			BlinkAttack();
			TeleportBehindTarget(targetActor, BlinkMoveBackDistance);
			return;
		}
		else if (targetMonster->GetAllowCounterAttack_Repulse())// && isPressed_S)
		{
			RepulseAttack();
			return;
		}
	}

	isPressShift = true;
	PressShiftTime = GetWorld()->GetTimeSeconds();
}

void AEveCharacter::AttackFinished(const float ComboResetDelay)
{
	//UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true);
		isAttacking = false;
	}
	// ComboResetDelay 후에 콤보 시퀀스 종료
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
	Idle();
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

