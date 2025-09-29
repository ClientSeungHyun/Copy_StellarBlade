// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_HarassMove.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/Monster_AnimInstance.h"

EBTNodeResult::Type UBTTaskNode_HarassMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FVector MoveDirection = CalculateHarassDirection(OwnerComp);
    OwnerComp.GetBlackboardComponent()->SetValueAsVector("HarassDirection", MoveDirection);


    return EBTNodeResult::Succeeded;
}

FVector UBTTaskNode_HarassMove::CalculateHarassDirection(UBehaviorTreeComponent& OwnerComp)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!Pawn) return FVector::ZeroVector;

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard) return FVector::ZeroVector;

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
    if (!TargetActor) return FVector::ZeroVector;

    FVector ToTarget = TargetActor->GetActorLocation() - Pawn->GetActorLocation();
    float DistanceToTarget = ToTarget.Size();
    ToTarget.Z = 0;
    ToTarget.Normalize();

    // 좌우 방향 계산
    FVector RightVector = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
    float SideSign = FMath::RandBool() ? 1.f : -1.f;
    float SideOffset = FMath::RandRange(0.5f, 1.0f);

    // 앞뒤 이동 결정
    float ForwardOffset = 0.0f;
    if (DistanceToTarget > DesiredDistance)
    {
        // 플레이어가 멀면 앞으로 이동
        ForwardOffset = FMath::RandRange(0.3f, 0.7f);
    }
    else if (DistanceToTarget < MinDistance)
    {
        // 너무 가까우면 뒤로 이동
        ForwardOffset = FMath::RandRange(-0.7f, -0.3f);
    }
    else
    {
        // 적정 거리일 때는 앞뒤 이동 없음
        ForwardOffset = 0.0f;
    }

    // 최종 방향 계산
    FVector HarassDir = (ToTarget * ForwardOffset) + (RightVector * SideSign * SideOffset);
    HarassDir.Normalize();

    return HarassDir;
}
