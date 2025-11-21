// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_MovePosition.h"
#include "AnimNotifyState_MovePosition.h"


#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/MonsterAIController.h"

void UAnimNotifyState_MovePosition::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp) return;

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        StartLocation = Character->GetActorLocation();
        ElapsedTime = 0.f;
        MoveDuration = TotalDuration;

        if (AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(Character->GetController()))
        {
            MonsterAIController->SetIsLookingPlayer(isLookingPlayer);
        }
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


        FVector Direction = FVector::ZeroVector;
        switch (MoveDirection)
        {
        case EMoveDirection::Front:
            Direction = Character->GetActorForwardVector();
            break;
        case EMoveDirection::Back:
            Direction = -Character->GetActorForwardVector();
            break;
        case EMoveDirection::Left:
            Direction = -Character->GetActorRightVector();
            break;
        case EMoveDirection::Right:
            Direction = Character->GetActorRightVector();
            break;
        default:
            break;
        }

        Direction.Z = 0.f;
        Direction += AdditionalMoveDirection.X * Character->GetActorForwardVector();
        Direction += AdditionalMoveDirection.Y * Character->GetActorRightVector();
        Direction.Normalize();

        FVector TargetLocation = StartLocation + Direction * MoveDistance * Alpha;
        Character->SetActorLocation(TargetLocation, true); // 충돌 고려
    }
}

void UAnimNotifyState_MovePosition::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
        if (AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(Character->GetController()))
        {
            MonsterAIController->SetIsLookingPlayer(false);
        }
    }
}
