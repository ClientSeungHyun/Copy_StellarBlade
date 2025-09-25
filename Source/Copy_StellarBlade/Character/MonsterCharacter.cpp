// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterCharacter.h"

#include "SBGameplayTags.h"
#include "SBDefine.h"
#include "GameplayTagContainer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SBAttributeComponent.h"
#include "Components/SBStateComponent.h"
#include "Components/SBCombatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/RotationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Equipments/SBWeapon.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "UI/SBStatBarWidget.h"
#include "UI/HealthBarWidget.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;



	// HealthBar
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	HealthBarWidgetComponent->SetDrawSize(FVector2D(100.f, 5.f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<USBAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<USBStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<USBCombatComponent>(TEXT("Combat"));
	RotationComponent = CreateDefaultSubobject<URotationComponent>("RotationComponent");

	// OnDeath Delegate에 함수 바인딩.
	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);

	AttributeComponent->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	bUseControllerRotationYaw = false;

	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, HealthBarOffsetPosY));

	if (!DefaultWeaponClass.IsEmpty())
	{
		// 기본 무기 Array에서 2개까지만 서브 웨폰으로 지정
		// 0번은 Main 1번은 Sub
		if (!DefaultWeaponClass.IsEmpty())
		{
			int32 WeaponCount = 0;
			for (int i = 0; i < DefaultWeaponClass.Num(); ++i)
			{
				if (DefaultWeaponClass[i])
				{
					FActorSpawnParameters Params;
					Params.Owner = this;

					ASBWeapon* Weapon = GetWorld()->SpawnActor<ASBWeapon>(DefaultWeaponClass[i], GetActorTransform(), Params);
					CombatComponent->SetCombatEnabled(true);
					Weapon->EquipItem((bool)WeaponCount);
					++WeaponCount;
				}

				if (WeaponCount > 1)
					break;
			}
			
		}
	}

	SetupHealthBar();
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AMonsterCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
		GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Damaged : %f"), ActualDamage));
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		// 데미지 방향
		FVector ShotDirection = PointDamageEvent->ShotDirection;
		// 히트 위치 (표면 접촉 관점)
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		// 히트 방향
		FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
		// 히트한 객체의 Location (객체 중심 관점)
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
}

void AMonsterCharacter::OnDeath()
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Ragdoll
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName("Ragdoll");
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		MeshComp->SetSimulatePhysics(true);
	}
}

void AMonsterCharacter::OnAttributeChanged(ESBAttributeType AttributeType, float InValue)
{
	if (AttributeType == ESBAttributeType::Health)
	{
		if (HealthBarWidgetComponent)
		{
			if (UHealthBarWidget* StatBar = Cast<UHealthBarWidget>(HealthBarWidgetComponent->GetWidget()))
			{
				//StatBar->SetRatio(InValue);
			}
		}
	}
}

void AMonsterCharacter::SetupHealthBar()
{
	if (HealthBarWidgetComponent)
	{
		if (UHealthBarWidget* StatBar = Cast<UHealthBarWidget>(HealthBarWidgetComponent->GetWidget()))
		{
			// 컬러 설정.
			//StatBar->FillColorAndOpacity = FLinearColor::Red;
		}
	}

	// 체력 초기값 설정.
	if (AttributeComponent)
	{
		AttributeComponent->BroadcastAttributeChanged(ESBAttributeType::Health);
	}

}

void AMonsterCharacter::ImpactEffect(const FVector& Location)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void AMonsterCharacter::HitReaction(const AActor* Attacker)
{
	check(CombatComponent)

	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
	{
		PlayAnimMontage(HitReactAnimMontage);
	}
}


void AMonsterCharacter::ActivateWeaponCollision(EWeaponType InWeaponType)
{
	if (CombatComponent)
	{
		switch (InWeaponType)
		{
		case EWeaponType::MainWeapon:
			CombatComponent->GetMainWeapon()->ActivateCollision();
			break;

		case EWeaponType::SubWeapon:
			CombatComponent->GetSubWeapon()->ActivateCollision();
			break;
		}
	}
}

void AMonsterCharacter::DeactivateWeaponCollision(EWeaponType InWeaponType)
{
	if (CombatComponent)
	{
		switch (InWeaponType)
		{
		case EWeaponType::MainWeapon:
			CombatComponent->GetMainWeapon()->DeactivateCollision();
			break;

		case EWeaponType::SubWeapon:
			CombatComponent->GetSubWeapon()->DeactivateCollision();
			break;
		}
	}
}

void AMonsterCharacter::PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate)
{
	check(StateComponent)
	check(AttributeComponent)
	check(CombatComponent)

	if (const ASBWeapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(SBGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		AttributeComponent->ToggleStaminaRegeneration(false);

		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag))
		{
			if (UAnimInstance* AnimInstance = ACharacter::GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}

		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag);
		AttributeComponent->DecreaseStamina(StaminaCost);
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

void AMonsterCharacter::ToggleHealthBarVisibility(bool bVisibility)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(bVisibility);
	}
}
