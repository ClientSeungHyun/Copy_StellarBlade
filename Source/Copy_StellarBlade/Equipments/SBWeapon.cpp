 //Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/SBWeapon.h"
#include "SBWeapon.h"

#include "SBGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SBWeaponCollisionComponent.h"
#include "Components/SBCombatComponent.h"
#include "Animations/SBAnimInstance.h"
#include "GameFramework/Character.h"
#include "Data/SBMontageActionData.h"

ASBWeapon::ASBWeapon()
{
	for (int i = 0; i < WeaponCollisionCount; ++i)
	{
		USBWeaponCollisionComponent* WeaponCollision = CreateDefaultSubobject<USBWeaponCollisionComponent>("MainCollision");
		WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

		WeaponCollisions.Add(WeaponCollision);
	}

	StaminaCostMap.Add(SBGameplayTags::Character_Attack_Light, 7.f);
	StaminaCostMap.Add(SBGameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(SBGameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(SBGameplayTags::Character_Attack_Heavy, 20.f);

	DamageMultiplierMap.Add(SBGameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(SBGameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(SBGameplayTags::Character_Attack_Special, 2.1f);
}

void ASBWeapon::EquipItem(bool isSubWeapon)
{
	Super::EquipItem();

	CombatComponent = GetOwner()->GetComponentByClass<USBCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this, isSubWeapon);

		const FName AttachSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;

		AttachToOwner(AttachSocket);

		for (auto& WeaponCollision : WeaponCollisions)
		{
			WeaponCollision->SetWeaponMesh(Mesh);
			WeaponCollision->AddIgnoredActor(GetOwner());
		}

		// 장착한 무기의 CombatType으로 업데이트.
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (USBAnimInstance* Anim = Cast<USBAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				//Anim->UpdateCombatMode(CombatType);
			}
		}
	}
}

UAnimMontage* ASBWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

UAnimMontage* ASBWeapon::GetRandomMontageForTag(const FGameplayTag& Tag) const
{
	return MontageActionData->GetRandomMontageForTag(Tag);
}
//
UAnimMontage* ASBWeapon::GetHitReactMontage(const AActor* Attacker) const
{
	// LookAt 회전값을 구합니다. (현재 Actor가 공격자를 바라보는 회전값)
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());
	// 현재 Actor의 회전값과 LookAt 회전값의 차이를 구합니다.
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetOwner()->GetActorRotation(), LookAtRotation);
	// Z축 기준의 회전값 차이만을 취합니다.
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 45.f))
	{
		HitDirection = EHitDirection::Front;
		UE_LOG(LogTemp, Log, TEXT("Front"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f))
	{
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f))
	{
		HitDirection = EHitDirection::Back;
		UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f))
	{
		HitDirection = EHitDirection::Right;
		UE_LOG(LogTemp, Log, TEXT("Right"));
	}

	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection)
	{
	case EHitDirection::Front:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 0);
		break;
	case EHitDirection::Back:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 1);
		break;
	case EHitDirection::Left:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 2);
		break;
	case EHitDirection::Right:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 3);
		break;
	}

	return SelectedMontage;
}

float ASBWeapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag))
	{
		return StaminaCostMap[InTag];
	}
	return 0.f;
}

float ASBWeapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType();

		if (DamageMultiplierMap.Contains(LastAttackType))
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType];
			return BaseDamage * Multiplier;
		}
	}

	return BaseDamage;
}

void ASBWeapon::OnHitActor(const FHitResult& Hit)
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

void ASBWeapon::ActivateCollision()
{
	if (WeaponCollisions.IsEmpty())
		return;

	for (const auto& Col : WeaponCollisions)
		Col->TurnOnCollision();
}

void ASBWeapon::DeactivateCollision()
{
	if (WeaponCollisions.IsEmpty())
		return;

	for (const auto& Col : WeaponCollisions)
		Col->TurnOffCollision();
}
