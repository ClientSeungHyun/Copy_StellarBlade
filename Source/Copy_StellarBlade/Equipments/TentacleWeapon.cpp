// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/TentacleWeapon.h"

#include "Animations/SBAnimInstance.h"
#include "Character/MonsterCharacter.h"
#include "Components/SBCombatComponent.h"
#include "Components/SBWeaponCollisionComponent.h"

ATentacleWeapon::ATentacleWeapon()
{
}

void ATentacleWeapon::BeginPlay()
{
}

void ATentacleWeapon::EquipItem()
{
	CombatComponent = GetOwner()->GetComponentByClass<USBCombatComponent>();

	if (CombatComponent)
	{
		CombatComponent->SetWeapon(this);

		if (AMonsterCharacter* OwnerCharacter = Cast<AMonsterCharacter>(GetOwner()))
		{
			WeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());
			SecondWeaponCollision->SetWeaponMesh(OwnerCharacter->GetMesh());

			CombatComponent->SetCombatEnabled(true);

			// 장착한 무기의 CombatType으로 업데이트.
			if (USBAnimInstance* Anim = Cast<USBAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
			{
				Anim->UpdateCombatMode(CombatType);
			}

			WeaponCollision->AddIgnoredActor(OwnerCharacter);
			SecondWeaponCollision->AddIgnoredActor(OwnerCharacter);
		}
	}
}
