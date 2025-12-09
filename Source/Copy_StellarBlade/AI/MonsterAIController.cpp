// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MonsterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Equipments/SBWeapon.h"
#include "Animation/Monster_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterAIController::AMonsterAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerctption");
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<AMonsterCharacter>(InPawn);

	RunBehaviorTree(BehaviorTreeAsset);

	bIsLookingPlayer = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateTarget, 0.1f, true);
}

void AMonsterAIController::OnUnPossess()
{
	ControlledEnemy = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Super::OnUnPossess();
}

void AMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledEnemy)
		return;

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!Blackboard)
		return;
	
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
	if (TargetActor)
	{
		FVector Direction = (TargetActor->GetActorLocation() - ControlledEnemy->GetActorLocation()).GetSafeNormal2D();
		FRotator TargetRotation = Direction.Rotation();

		if (bIsLookingPlayer)
		{
			FRotator NewRotation = FMath::RInterpTo(ControlledEnemy->GetActorRotation(), TargetRotation, DeltaTime, 3.0f);
			ControlledEnemy->SetActorRotation(NewRotation);
		}
	}

	if (BlackboardComp->GetValueAsEnum("Behavior") == (uint8)EMonsterAIBehavior::Attack
		&& Cast<UMonster_AnimInstance>(ControlledEnemy->GetMesh()->GetAnimInstance())->IsHarassing())
	{
		FVector Direction = BlackboardComp->GetValueAsVector("HarassDirection");
		Direction.Z = 0.f;
		Direction = ControlledEnemy->GetActorRotation().RotateVector(Direction);
		Direction.Normalize();

		ControlledEnemy->AddMovementInput(Direction, 1.f);
		ControlledEnemy->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void AMonsterAIController::UpdateTarget()
{
	TArray<AActor*> OutActors;
	AIPerceptionComponent->GetKnownPerceivedActors(nullptr, OutActors);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (OutActors.Contains(PlayerCharacter))
	{
		SetTarget(PlayerCharacter);
		ControlledEnemy->ToggleMonsterStateVisibility(true);
	}
	else
	{
		SetTarget(nullptr);
		ControlledEnemy->ToggleMonsterStateVisibility(false);
	}

}

void AMonsterAIController::SetTarget(AActor* NewTarget)
{
	if (!IsValid(Blackboard))
		return;

	AActor* PreviousTarget = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
	Blackboard->SetValueAsObject(FName("Target"), NewTarget);

	if (PreviousTarget == nullptr && IsValid(NewTarget))
	{
		HandleBattleStart();
	}

	if (OnTargetChange.IsBound())
	{
		bHaveTarget = NewTarget != nullptr;
		OnTargetChange.Broadcast(bHaveTarget);
	}

}

void AMonsterAIController::HandleBattleStart()
{
	AMonsterCharacter* Monster = Cast<AMonsterCharacter>(GetPawn());
	if (!Monster)
		return;

	const ASBWeapon* Weapon = Monster->GetMainWeapon();
	if (!Weapon)
	{
		Monster->SetCombatEnabled(true);
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		Monster->SetCombatEnabled(true);
		return;
	}

	PlayBattleStartMontage(Monster, Weapon, AnimInstance);
}

void AMonsterAIController::PlayBattleStartMontage(AMonsterCharacter* Monster, const ASBWeapon* Weapon, UAnimInstance* AnimInstance)
{
	ControlledEnemy->GetCharacterMovement()->DisableMovement();

	UAnimMontage* Montage = Weapon->GetMontageForTag(SBGameplayTags::Monster_State_Discovered, 0);
	if (!Montage)
	{
		Monster->SetCombatEnabled(true);
		return;
	}

	float Duration = AnimInstance->Montage_Play(Montage);
	if (Duration > 0.f)
	{
		// 현재 실행중인 애니메이션 몽타주가 끝나면 실행되는 델리게이트를 바인드
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &AMonsterAIController::OnBattleStartMontageEnded, Monster);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	}
	else
	{
		Monster->SetCombatEnabled(true);
	}
}

void AMonsterAIController::OnBattleStartMontageEnded(UAnimMontage* Montage, bool bInterrupted, AMonsterCharacter* Monster)
{
	if (Monster)
	{
		if (ControlledEnemy)
		{
			ControlledEnemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			Monster->SetCombatEnabled(true);
		}
	}
}

void AMonsterAIController::SetIsLookingPlayer(bool InIsLookingTarget)
{
	bIsLookingPlayer = InIsLookingTarget;
}
