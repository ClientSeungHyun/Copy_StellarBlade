// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Monster_AnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/EveCharacter.h"
#include "Components/SBStateComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SBCombatComponent.h"
#include "AI/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/MonsterAIController.h"

UMonster_AnimInstance::UMonster_AnimInstance()
{
}

void UMonster_AnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ACharacter>(GetOwningActor());

    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();

        if (USBCombatComponent* CombatComponent = Character->GetComponentByClass<USBCombatComponent>())
        {
            CombatComponent->OnChangedCombat.AddUObject(this, &ThisClass::OnChangedCombat);
        }
    }
}

void UMonster_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!Character)
        return;
    if (!MovementComponent)
        return;
    
    if (MonsterAIController)
    {
        UBlackboardComponent* Blackboard = MonsterAIController->GetBlackboardComponent();
        if (!Blackboard) return;

        Direction = Blackboard->GetValueAsVector("HarassDirection");

        bIsHarassing = Blackboard->GetValueAsEnum("Behavior") == (uint8)EMonsterAIBehavior::Harass;
    }

    Velocity = MovementComponent->Velocity;

    GroundSpeed = FMath::FInterpTo(GroundSpeed, Velocity.Size2D(), DeltaSeconds, 7.f);

    //FVector Acc = MovementComponent->GetCurrentAcceleration();
    bShouldMove = GroundSpeed > 3.f && Velocity != FVector::ZeroVector;

    bIsFalling = MovementComponent->IsFalling();

    //Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
}

void UMonster_AnimInstance::AnimNotify_ResetMovementInput()
{
    if (const AEveCharacter* LocalCharacter = Cast<AEveCharacter>(GetOwningActor()))
    {
        LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
    }
}

void UMonster_AnimInstance::AnimNotify_ResetState()
{
    if (const AEveCharacter* LocalCharacter = Cast<AEveCharacter>(GetOwningActor()))
    {
        LocalCharacter->GetStateComponent()->ClearState();
    }
}

void UMonster_AnimInstance::UpdateCombatMode(const ECombatType InCombatType)
{
    CombatType = InCombatType;
}

void UMonster_AnimInstance::BindAIController(AController* NewController)
{
    MonsterAIController = Cast<AMonsterAIController>(NewController);
    if (MonsterAIController)
    {
        MonsterAIController->OnTargetChange.AddUObject(this, &UMonster_AnimInstance::OnChangedTarget);
    }
}

void UMonster_AnimInstance::OnChangedCombat(const bool bInCombatEnabled)
{
    bCombatEnabled = bInCombatEnabled;
}

void UMonster_AnimInstance::OnChangedTarget(const bool bInHaveTarget)
{
    bHaveTarget = bInHaveTarget;
}