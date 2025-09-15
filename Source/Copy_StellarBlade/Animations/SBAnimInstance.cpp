// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SBAnimInstance.h"

#include "Character/SBCharacter.h"
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

    bShouldMove = GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;

    bIsFalling = MovementComponent->IsFalling();
}

void USBAnimInstance::AnimNotify_ResetMovementInput()
{
    //if (const ASBCharacter* LocalCharacter = Cast<ASBCharacter>(GetOwningActor()))
    //{
    //    LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
    //}
}

void USBAnimInstance::AnimNotify_ResetState()
{
    //if (const ASBCharacter* LocalCharacter = Cast<ASBCharacter>(GetOwningActor()))
    //{
    //    LocalCharacter->GetStateComponent()->ClearState();
    //}
}
