// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/TentacleWeapon.h"

#include "Animation/Monster_AnimInstance.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Components/SBCombatComponent.h"
#include "Components/SBWeaponCollisionComponent.h"

ATentacleWeapon::ATentacleWeapon()
{
}

void ATentacleWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponCollisionCount = Array_TraceStartEndName.Num() - 1;

	for (int32 i = 0; i < WeaponCollisionCount; i++)
	{
		USBWeaponCollisionComponent* WeaponCollision{};

		if (WeaponCollisions.Num() > i)
		{
			WeaponCollision = WeaponCollisions[i];

			if (Array_TraceStartEndName.Num() > i + 1)
				WeaponCollision->SetTraceName(Array_TraceStartEndName[i], Array_TraceStartEndName[i + 1]);

			WeaponCollision->SetAttachmentType(EAttachmentType::Bone);
			WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);

			continue;
		}

		FName CollisionName = FName(*FString::Printf(TEXT("Collision%d"), i + 1));
		WeaponCollision = NewObject<USBWeaponCollisionComponent>(this, USBWeaponCollisionComponent::StaticClass(), CollisionName);
		WeaponCollision->RegisterComponent();

		if (WeaponCollision)
		{
			if (Array_TraceStartEndName.Num() > i + 1)
				WeaponCollision->SetTraceName(Array_TraceStartEndName[i], Array_TraceStartEndName[i + 1]);

			WeaponCollision->SetAttachmentType(EAttachmentType::Bone);
			WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);
			WeaponCollisions.Add(WeaponCollision);
		}
	}
}

void ATentacleWeapon::EquipItem(bool isSubWeapon)
{
	CombatComponent = GetOwner()->GetComponentByClass<USBCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this, isSubWeapon);

		if (AMonsterCharacter* OwnerCharacter = Cast<AMonsterCharacter>(GetOwner()))
		{
			for (auto& WeaponCollision : WeaponCollisions)
			{
				WeaponCollision->SetOwnerWeaopon(this);
				WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
				WeaponCollision->AddIgnoredActor(OwnerCharacter);
			}

			// 장착한 무기의 CombatType으로 업데이트.
			if (UMonster_AnimInstance* Anim = Cast<UMonster_AnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}

		}
	}
}
