// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBEveWeapon.h"
#include "SBEveTags.h"
#include "Data/SBMontageActionData.h"
#include "Components/SBWeaponCollisionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EveCharacter.h"
#include "Components/SBEveAtrributeComponent.h"

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
}

void ASBEveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			Owner->GetAttributeComponent()->AddBetaEnergy();
			
			return BaseDamage * Multiplier;
		}
	}

	return BaseDamage;
}
void ASBEveWeapon::ActivateCollision()
{
	WeaponCollision->TurnOnCollision();
}

void ASBEveWeapon::DeactivateCollision()
{
	WeaponCollision->TurnOffCollision();
}

void ASBEveWeapon::OnHitActor(const FHitResult& Hit)
{
	AActor* TargetActor = Hit.GetActor();

	// 데미지 방향
	FVector DamageDirection = GetOwner()->GetActorForwardVector();

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

