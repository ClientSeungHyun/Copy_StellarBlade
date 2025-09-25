// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SBAnimInstance.h"

#include "Character/EveCharacter.h"
#include "Components/SBStateComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USBAnimInstance::USBAnimInstance()
{
}

void USBAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ACharacter>(GetOwningActor());

    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();

        MovementComponent->RotationRate = FRotator(0.f, 0.f, 20.f); // 초당 회전 속도
        MovementComponent->bUseControllerDesiredRotation = false;
        MovementComponent->bOrientRotationToMovement = true;
    }
}

void USBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (Character == nullptr)
    {
        return;
    }

    if (MovementComponent == nullptr)
    {
        return;
    }

    Velocity = MovementComponent->Velocity;
    GroundSpeed = Velocity.Size2D();

    //FVector Acc = MovementComponent->GetCurrentAcceleration();
    bShouldMove = GroundSpeed > 3.f && Velocity != FVector::ZeroVector;

    bIsFalling = MovementComponent->IsFalling();
}

void USBAnimInstance::AnimNotify_ResetMovementInput()
{
    if (const AEveCharacter* LocalCharacter = Cast<AEveCharacter>(GetOwningActor()))
    {
        LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
    }
}

void USBAnimInstance::AnimNotify_ResetState()
{
    if (const AEveCharacter* LocalCharacter = Cast<AEveCharacter>(GetOwningActor()))
    {
        LocalCharacter->GetStateComponent()->ClearState();
    }
}

void USBAnimInstance::UpdateCombatMode(const ECombatType InCombatType)
{
    CombatType = InCombatType;
}

void USBAnimInstance::OnChangedCombat(const bool bInCombatEnabled)
{
    bCombatEnabled = bInCombatEnabled;
}
