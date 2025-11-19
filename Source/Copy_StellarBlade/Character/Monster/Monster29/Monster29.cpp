// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Monster29/Monster29.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsAsset.h"

AMonster29::AMonster29()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMonster29::OnDeath()
{
	if (bIsDead)
		return;

	bIsDead = true;

	// AI 중지
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
		AIController->UnPossess();
	}



	// 피직스 에셋 문제인지 모르겠지만 로봇은 Disslove로 사라지게 해야 할 듯
 //   // 기존 메시 숨기기
 //   if (USkeletalMeshComponent* MeshComp = GetMesh())
 //   {
 //       MeshComp->SetVisibility(false, true);
 //       MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 //       MeshComp->SetSimulatePhysics(false);
 //   }

	//FVector BaseLoc = GetMesh()->GetComponentLocation();
	//FRotator BaseRot = GetMesh()->GetComponentRotation();

	//// 공통 스폰 함수
	//auto SpawnBodyPart = [&](USkeletalMesh* MeshAsset, const FVector& Offset, const FVector& Impulse)
	//	{
	//		if (!MeshAsset) return;

	//		FActorSpawnParameters Params;
	//		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//		ASkeletalMeshActor* BodyPart = GetWorld()->SpawnActor<ASkeletalMeshActor>(
	//			ASkeletalMeshActor::StaticClass(),
	//			BaseLoc ,
	//			BaseRot,
	//			Params
	//		);

	//		if (BodyPart && BodyPart->GetSkeletalMeshComponent())
	//		{
	//			USkeletalMeshComponent* Comp = BodyPart->GetSkeletalMeshComponent();
	//			Comp->SetSkeletalMesh(MeshAsset);
	//			//Comp->SetCollisionProfileName(TEXT("Ragdoll")); // 충돌 및 물리 프로필 활성화
	//			Comp->SetSimulatePhysics(true);                 // 물리 활성화 (중력, 관성 적용)
	//			Comp->WakeAllRigidBodies();                     // 즉시 물리 반응

	//			Comp->SetEnableGravity(true);
	//			Comp->AddImpulse(Impulse * Comp->GetMass());    // 관성 적용
	//		}
	//	};

	//// === 상체 ===
	//const FVector UpperImpulse = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 1.2f) * 100.f;
	//SpawnBodyPart(UpperBodyMesh, FVector(0, 0, 0), UpperImpulse);

	//// === 하체 ===
	//const FVector LowerImpulse = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.5f) * 100.f;
	//SpawnBodyPart(LowerBodyMesh, FVector(0, 0, 0), LowerImpulse);

	//// 디버그 표시
	//UKismetSystemLibrary::PrintString(this, TEXT("몬스터 29 사망 - 분리된 액터 스폰됨"), true, true, FLinearColor::Red, 3.0f);
}