// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_MovePosition.h"
#include "AnimNotifyState_MovePosition.h"


#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_MovePosition::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp) return;

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        if (TotalDuration > 0.f)
        {
            MoveSpeed = MoveDistance / TotalDuration;
        }
        else
        {
            MoveSpeed = 0.f;
        }

        Character->GetCharacterMovement()->MaxWalkSpeed = 1000.f;
    }
}

void UAnimNotifyState_MovePosition::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!MeshComp) return;

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        MoveDirection = Character->GetActorForwardVector();
        MoveDirection.Z = 0.f;
        MoveDirection.Normalize();

        Character->AddMovementInput(MoveDirection, MoveSpeed * FrameDeltaTime);
    }
}

void UAnimNotifyState_MovePosition::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
