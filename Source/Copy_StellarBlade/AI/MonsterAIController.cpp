// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MonsterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/MonsterCharacter.h"
#include "Equipments/SBWeapon.h"

AMonsterAIController::AMonsterAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerctption");
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<AMonsterCharacter>(InPawn);

	RunBehaviorTree(BehaviorTreeAsset);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateTarget, 0.1f, true);
}

void AMonsterAIController::OnUnPossess()
{
	ControlledEnemy = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Super::OnUnPossess();
}

void AMonsterAIController::UpdateTarget()
{
	TArray<AActor*> OutActors;
	AIPerceptionComponent->GetKnownPerceivedActors(nullptr, OutActors);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (OutActors.Contains(PlayerCharacter))
	{
		SetTarget(PlayerCharacter);
		ControlledEnemy->ToggleHealthBarVisibility(true);
	}
	else
	{
		SetTarget(nullptr);
		ControlledEnemy->ToggleHealthBarVisibility(false);
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
	UAnimMontage* Montage = Weapon->GetMontageForTag(SBGameplayTags::Character_State_Discovered, 0);
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
		Monster->SetCombatEnabled(true);
	}
}