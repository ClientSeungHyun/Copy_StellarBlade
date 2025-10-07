 //Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/SBWeapon.h"
#include "SBWeapon.h"

#include "SBGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SBWeaponCollisionComponent.h"
#include "Components/SBCombatComponent.h"
#include "Animation/Monster_AnimInstance.h"
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

	DamageMultiplierMap.Add(SBGameplayTags::Monster_Attack_General, 1.8f);
	DamageMultiplierMap.Add(SBGameplayTags::Monster_Attack_Blink, 1.8f);
	DamageMultiplierMap.Add(SBGameplayTags::Monster_Attack_Repulse, 2.1f);
}

void ASBWeapon::EquipItem(bool isSubWeapon)
{
	Super::EquipItem();

	CombatComponent = GetOwner()->GetComponentByClass<USBCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this, isSubWeapon);

		const FName AttachSocket = EquipSocketName;

		AttachToOwner(AttachSocket);

		for (auto& WeaponCollision : WeaponCollisions)
		{
			WeaponCollision->SetWeaponMesh(Mesh);
			WeaponCollision->AddIgnoredActor(GetOwner());
		}

		// 장착한 무기의 CombatType으로 업데이트.
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			if (UMonster_AnimInstance* Anim = Cast<UMonster_AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
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
	// 현재 Actor가 공격자를 바라보는 회전값
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());
	// 현재 Actor의 회전값과 LookAt 회전값의 차이를 구합니다.
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetOwner()->GetActorRotation(), LookAtRotation);
	// Z축 기준의 회전값 차이만을 취합니다.
	const float DeltaZ = DeltaRotation.Yaw;

	EHitDirection HitDirection = EHitDirection::Front_L;

	if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -45.f, 0.f))
	{
		HitDirection = EHitDirection::Front_L;
		UE_LOG(LogTemp, Log, TEXT("Front_L"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 0.f, 45.f))
	{
		HitDirection = EHitDirection::Front_R;
		UE_LOG(LogTemp, Log, TEXT("Front_R"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 135.f, 180.f)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -180.f, -135.f))
	{
		HitDirection = EHitDirection::Back;
		UE_LOG(LogTemp, Log, TEXT("Back"));
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, 45.f, 135.f))
	{
		HitDirection = EHitDirection::Left;
		UE_LOG(LogTemp, Log, TEXT("Left"));
	}

	else if (UKismetMathLibrary::InRange_FloatFloat(DeltaZ, -135.f, -45.f))
	{
		HitDirection = EHitDirection::Right;
		UE_LOG(LogTemp, Log, TEXT("Right"));
	}

	UAnimMontage* SelectedMontage = nullptr;
	switch (HitDirection)
	{
	case EHitDirection::Front_L:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 0);
		break;
	case EHitDirection::Front_R:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 1);
		break;
	case EHitDirection::Back:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 2);
		break;
	case EHitDirection::Left:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 3);
		break;
	case EHitDirection::Right:
		SelectedMontage = GetMontageForTag(SBGameplayTags::Character_Action_HitReaction, 4);
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

void ASBWeapon::ActivateWeapon()
{
	// 무기 본 인덱스 찾기
	int32 BoneIndex = Mesh->GetBoneIndex(FName("Bip001-R-Sword"));
	if (BoneIndex != INDEX_NONE)
	{
		FTransform BoneTransform = Mesh->GetBoneTransform(BoneIndex);

		// 길이 늘리기 (Scale 조정)
		FVector NewScale = BoneTransform.GetScale3D();
		NewScale.Z = 0.5f; // 활성화 시 2배
		BoneTransform.SetScale3D(NewScale);

		//// 본 트랜스폼 적용
		//Mesh->BoneSpaceTransforms[BoneIndex] = BoneTransform.ToMatrixWithScale();
		//Mesh->MarkRefreshTransformDirty(); // 갱신 필요
	}
}

void ASBWeapon::DeactiveWeapon()
{
}
