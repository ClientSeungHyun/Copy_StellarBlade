// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBEveWeapon.h"
#include "SBEveTags.h"
#include "Data/SBMontageActionData.h"
#include "Components/SBWeaponCollisionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EveCharacter.h"
#include "Components/SBEveAtrributeComponent.h"
#include "Sound/SoundBase.h"

// Sets default values
ASBEveWeapon::ASBEveWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	WeaponCollision = CreateDefaultSubobject<USBWeaponCollisionComponent>("WeaponCollision");
	WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

	WeaponCollision->SetWeaponMesh(SwordMesh);

	DamageMultiplierMap.Add(SBEveTags::Eve_Attack_NormalAttack, 1.8f);
	DamageMultiplierMap.Add(SBEveTags::Eve_Attack_SkillAttack, 2.1);

	
}

void ASBEveWeapon::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < 3; i++)
	{
		if (!Guard_effects[i]) continue;

		GuardComps[i] = NewObject<UNiagaraComponent>(this);
		GuardComps[i]->SetupAttachment(RootComponent);
		GuardComps[i]->SetAsset(Guard_effects[i]);
		GuardComps[i]->RegisterComponent();
		GuardComps[i]->SetAutoActivate(false);
		GuardComps[i]->SetAutoDestroy(false);
	}

	StopAllGuardEffects();
}

void ASBEveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateWeaponMoveDirection();
}

void ASBEveWeapon::CalculateWeaponMoveDirection()
{
	FVector CurrentPos = GetOwner()->GetActorLocation();

	AttackDirection = CurrentPos - PrevPosition;
	AttackDirection.Normalize();

	PrevPosition = CurrentPos;
}

void ASBEveWeapon::EquipItem()
{
	WeaponCollision->AddIgnoredActor(GetOwner());
}

void ASBEveWeapon::SetLastAttackTag(FGameplayTag tag)
{
	lastAttackTag = tag;
}

UAnimMontage* ASBEveWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

float ASBEveWeapon::GetAttackDamage()
{
	if (const AActor* OwnerActor = GetOwner())
	{
		if (DamageMultiplierMap.Contains(lastAttackTag))
		{
			const float Multiplier = DamageMultiplierMap[lastAttackTag];
			
			if (Owner == nullptr)
			{
				Owner = Cast<AEveCharacter>(GetOwner());
			}
			Owner->GetAttributeComponent()->AddBetaEnergy(3.f);
			
			return BaseDamage * Multiplier;
		}
	}

	return BaseDamage;
}
void ASBEveWeapon::PlayAllGuardEffects()
{
	for (int i = 0; i < 3; i++)
		if (GuardComps[i]) GuardComps[i]->Activate(true);
}

void ASBEveWeapon::StopAllGuardEffects()
{
	for (int i = 0; i < 3; i++)
		if (GuardComps[i]) GuardComps[i]->DeactivateImmediate();
}

void ASBEveWeapon::PlayBlinkEffect()
{
	if (Blink_effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			Blink_effect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(0.1f, 0.1f, 0.1f),
			true,   // bAutoDestroy
			true    // bAutoActivate
		);
	}
}

void ASBEveWeapon::ActivateCollision()
{
	WeaponCollision->TurnOnCollision();
}

void ASBEveWeapon::DeactivateCollision()
{
	WeaponCollision->TurnOffCollision();
}

void ASBEveWeapon::PlayAttackSound()
{
	if (AttackSoundList.Num() == 0) return;

	// 랜덤 인덱스 뽑기
	int32 RandomIndex = FMath::RandRange(0, AttackSoundList.Num() - 1);

	USoundBase* RandomSound = AttackSoundList[RandomIndex];
	if (RandomSound)
	{
		// 소리 재생
		UGameplayStatics::PlaySoundAtLocation(
			this,
			RandomSound,
			GetActorLocation()
		);
	}
}

void ASBEveWeapon::OnHitActor(const FHitResult& Hit)
{
	PlayAttackSound();

	AActor* TargetActor = Hit.GetActor();

	// 데미지 방향
	FVector DamageDirection = GetOwner()->GetActorForwardVector();
	//FVector DamageDirection = AttackDirection;

	// 데미지
	float AttackDamage = GetAttackDamage();

	UGameplayStatics::ApplyPointDamage(
		TargetActor,
		AttackDamage,
		DamageDirection,
		Hit,
		GetOwner()->GetInstigatorController(),
		this,
		nullptr);
}

FVector ASBEveWeapon::GetAttackDirection()
{
	return AttackDirection;
}

