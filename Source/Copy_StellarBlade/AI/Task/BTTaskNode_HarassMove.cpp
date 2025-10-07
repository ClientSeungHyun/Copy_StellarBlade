// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_HarassMove.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/Monster_AnimInstance.h"
#include "Character/Monster/MonsterCharacter.h"

EBTNodeResult::Type UBTTaskNode_HarassMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    const AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
    if (!TargetActor) return EBTNodeResult::Failed;;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    FVector CurrentPlayerLocation = TargetActor->GetActorLocation();

    const float UpdateInterval = FMath::FRandRange(4.f, 7.f);
    bool bTimePassed = (CurrentTime - LastDirectionUpdateTime) >= UpdateInterval;

    const float MinDistanceChange = 400.0f;
    bool bPlayerMoved = FVector::Dist(CurrentPlayerLocation, LastPlayerLocation) > MinDistanceChange;

    if (bTimePassed || bPlayerMoved)
    {
        CurrentHarassDirection = CalculateHarassDirection(OwnerComp);

        LastDirectionUpdateTime = CurrentTime;
        LastPlayerLocation = CurrentPlayerLocation;
    }

    // 이동 방향 적용
    OwnerComp.GetBlackboardComponent()->SetValueAsVector("HarassDirection", CurrentHarassDirection);

    if (AIController && AIController->GetPawn())
    {
        UMonster_AnimInstance* AnimInstance = Cast< UMonster_AnimInstance>(OwnerComp.GetAIOwner()->GetCharacter()->GetMesh()->GetAnimInstance());
        if (AnimInstance)
        {
            AnimInstance->SetIsHarassing(true);
        }
    }

    return EBTNodeResult::Succeeded;
}


EBTNodeResult::Type UBTTaskNode_HarassMove::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController && AIController->GetPawn())
    {
        UMonster_AnimInstance* AnimInstance = Cast< UMonster_AnimInstance>(OwnerComp.GetAIOwner()->GetCharacter()->GetMesh()->GetAnimInstance());
        if (AnimInstance)
        {
            AnimInstance->SetIsHarassing(false);
        }
    }

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
    ToTarget.Z = 0;
    ToTarget.Normalize();

    // 좌우 방향 계산
    FVector RightVector = Pawn->GetActorRightVector(); /*FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal()*/;
    float SideSign = FMath::RandBool() ? 1.f : -1.f;
    float SideOffset = FMath::RandRange(0.5f, 1.0f);

    // 최종 방향 계산 (앞뒤 이동 제거)
    FVector HarassDir = RightVector * SideSign;
    HarassDir.Normalize();

    return HarassDir;
}
