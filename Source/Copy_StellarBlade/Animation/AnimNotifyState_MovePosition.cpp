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
        StartLocation = Character->GetActorLocation();
        ElapsedTime = 0.f;
        MoveDuration = TotalDuration;
    }
}

void UAnimNotifyState_MovePosition::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!MeshComp) return;

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        ElapsedTime += FrameDeltaTime;
        float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.f, 1.f);

        FVector Direction = Character->GetActorForwardVector();
        Direction.Z = 0.f;
        Direction.Normalize();

        FVector TargetLocation = StartLocation + Direction * MoveDistance * Alpha;
        Character->SetActorLocation(TargetLocation, true); // 충돌 고려
    }
}

void UAnimNotifyState_MovePosition::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
