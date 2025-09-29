// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SB_Eve_AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SBStateComponent.h"
#include "SBEveTags.h"
#include "Character/EveCharacter.h"
#include "KismetAnimationLibrary.h"

USB_Eve_AnimInstance::USB_Eve_AnimInstance()
{
}

void USB_Eve_AnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Player = Cast<AEveCharacter>(GetOwningActor());

    if (Player)
    {
        MovementComponent = Player->GetCharacterMovement();
        PlayerStateComp = Player->GetStateComponent();
    }
}

void USB_Eve_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (Player == nullptr)
    {
        return;
    }

    if (MovementComponent == nullptr)
    {
        return;
    }

    Velocity = MovementComponent->Velocity;
    GroundSpeed = Velocity.Size2D();

    bIsFalling = MovementComponent->IsFalling();

   /* if (GroundSpeed > 500.f && bIsFalling == false){
        bIsRunning = false;
        bIsSprinting = true;
    }
     else if(GroundSpeed > 1.f && bIsFalling == false)
    {
        bIsRunning = true;
        bIsSprinting = false;
    }*/

    //UE_LOG(LogTemp, Warning, TEXT("bIsRunning: %d"), bIsRunning);
   // UE_LOG(LogTemp, Warning, TEXT("bIsJumpingStart: %d"), bIsJumpingStart);

    if (PlayerStateComp->GetCurrentState() == SBEveTags::Eve_State_JumpStart)
    {
        bIsJumpingStart = true;
        bIsRunning = false;
        bIsSprinting = false;
    }

    if (GroundSpeed < 1.f && bIsFalling == false)
    {
        PlayerStateComp->SetState(SBEveTags::Eve_State_Idle);
    }

    if (PlayerStateComp->GetCurrentState() == SBEveTags::Eve_State_Running){
        bIsRunning = false;
        bIsSprinting = true;
        bIsJumpingStart = false;
    }

    if(PlayerStateComp->GetCurrentState() == SBEveTags::Eve_State_Walking)
    {
        bIsRunning = true;
        bIsSprinting = false;
        bIsJumpingStart = false;
    }
 
    if (PlayerStateComp->GetCurrentState() == SBEveTags::Eve_State_Idle)
    {
        bIsJumpingStart = false;
        bIsRunning = false;
        bIsSprinting = false;
    }

    Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Player->GetActorRotation());
}
