// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBCombatComponent.h"

#include "Character/EveCharacter.h"
#include "Equipments/SBWeapon.h"
#include "Items/SBPickupItem.h"

USBCombatComponent::USBCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

}


void USBCombatComponent::BeginPlay()
{
    Super::BeginPlay();

}


void USBCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USBCombatComponent::SetWeapon(ASBWeapon* NewWeapon)
{
    // 이미 무기를 가지고 있으면 PickupItem으로 만들어서 떨군다.
    if (::IsValid(MainWeapon))
    {
        if (AEveCharacter* OwnerCharacter = Cast<AEveCharacter>(GetOwner()))
        {
            ASBPickupItem* PickupItem = GetWorld()->SpawnActorDeferred<ASBPickupItem>(ASBPickupItem::StaticClass(), OwnerCharacter->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
            PickupItem->SetEquipmentClass(MainWeapon->GetClass());
            PickupItem->FinishSpawning(GetOwner()->GetActorTransform());

            MainWeapon->Destroy();
        }
    }

    MainWeapon = NewWeapon;
}

void USBCombatComponent::SetCombatEnabled(const bool bEnabled)
{
    bCombatEnabled = bEnabled;
    if (OnChangedCombat.IsBound())
    {
        OnChangedCombat.Broadcast(bCombatEnabled);
    }
}

