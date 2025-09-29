// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBCombatComponent.h"

#include "Character/EveCharacter.h"
#include "Equipments/SBWeapon.h"
#include "Items/SBPickupItem.h"
#include "AI/MonsterAIController.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

USBCombatComponent::USBCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

}


void USBCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (::IsValid(MainWeapon))
    {
        MainWeapon->EquipItem();
    }

    if (Character && Character->GetController())
    {
        OwnerAIController = Cast<AAIController>(Character->GetController());
        if (OwnerAIController)
        {
            if (AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(OwnerAIController))
            {
                MonsterAIController->OnTargetChange.AddUObject(this, &ThisClass::OnChangedTarget);
            }
        }
    }
}


void USBCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 플레이어를 인식했을 때만 공격 대기 체크
    if (bHaveTarget && !bCanAttack)
    {
        AttackDelayTimer -= DeltaTime;

        if (AttackDelayTimer <= 0.f && bCombatEnabled)
        {
            bCanAttack = true;
            if (OwnerAIController)
            {
                if (UBlackboardComponent* BlackboardComp = OwnerAIController->GetBlackboardComponent())
                {
                    BlackboardComp->SetValueAsBool("bCanAttack", bCanAttack);
                }
            }
        }
    }
}

void USBCombatComponent::SetWeapon(ASBWeapon* NewWeapon, bool isSubWeapon)
{
    ASBWeapon*& Weapon = isSubWeapon ? SubWeapon : MainWeapon;

    // 이미 무기를 가지고 있으면 PickupItem으로 만들어서 떨군다.
    if (::IsValid(Weapon))
    {
        if (AEveCharacter* OwnerCharacter = Cast<AEveCharacter>(GetOwner()))
        {
            ASBPickupItem* PickupItem = GetWorld()->SpawnActorDeferred<ASBPickupItem>(ASBPickupItem::StaticClass(), OwnerCharacter->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
            PickupItem->SetEquipmentClass(Weapon->GetClass());
            PickupItem->FinishSpawning(GetOwner()->GetActorTransform());

            Weapon->Destroy();
        }
    }

    Weapon = NewWeapon;
}

void USBCombatComponent::FinishAttack()
{
    bCanAttack = false;
    AttackDelayTimer = FMath::FRandRange(MinAttackTime, MaxAttackTime);

    if (OwnerAIController)
    {
        if (UBlackboardComponent* BlackboardComp = OwnerAIController->GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsBool("bCanAttack", bCanAttack);
        }
    }
}

void USBCombatComponent::SetCombatEnabled(const bool bEnabled)
{
    bCombatEnabled = bEnabled;
    if (OnChangedCombat.IsBound())
    {
        OnChangedCombat.Broadcast(bCombatEnabled);
    }
}

void USBCombatComponent::OnChangedTarget(const bool bInHaveTarget)
{
    bHaveTarget = bInHaveTarget;
}

