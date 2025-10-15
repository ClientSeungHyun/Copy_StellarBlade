// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterCharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SBGameplayTags.h"
#include "SBDefine.h"
#include "GameplayTagContainer.h"
#include "Components/CapsuleComponent.h"
#include "Components/MonsterAttributeComponent.h"
#include "Components/SBStateComponent.h"
#include "Components/SBCombatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/RotationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Equipments/SBWeapon.h"
#include "Animation/Monster_AnimInstance.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "UI/SBStatBarWidget.h"
#include "UI/Monster/MonsterStatBarWidget.h"

#include "ProceduralMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ProceduralMeshComponent/Public/KismetProceduralMeshLibrary.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Targeting 구체 생성및 Collision 설정.
	TargetingSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	TargetingSphereComponent->SetupAttachment(GetRootComponent());
	TargetingSphereComponent->SetCollisionObjectType(COLLISION_OBJECT_TARGETING);
	TargetingSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetingSphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// LockOn 위젯.
	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(GetRootComponent());
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	LockOnWidgetComponent->SetDrawSize(FVector2D(30.f, 30.f));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetVisibility(false);

	MonsterStatBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MonsterStatBarWidget"));
	MonsterStatBarWidget->SetupAttachment(GetRootComponent());
	MonsterStatBarWidget->SetDrawSize(FVector2D(200.f, 10.f));
	MonsterStatBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	MonsterStatBarWidget->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	AttributeComponent = CreateDefaultSubobject<UMonsterAttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<USBStateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<USBCombatComponent>(TEXT("Combat"));
	RotationComponent = CreateDefaultSubobject<URotationComponent>("RotationComponent");

	AttributeComponent->OnDeath.AddUObject(this, &ThisClass::OnDeath);
}

void AMonsterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (UMonster_AnimInstance* AnimInst = Cast<UMonster_AnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInst->BindAIController(NewController);
	}
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	bUseControllerRotationYaw = false;

	if (MonsterStatBarWidget)
	{
		UMonsterStatBarWidget* Widget = Cast<UMonsterStatBarWidget>(MonsterStatBarWidget->GetUserWidgetObject());
		if (Widget)
		{
			Widget->SetOwnerMonster(this);
		}
	}
	MonsterStatBarWidget->SetRelativeLocation(FVector(0.f, 0.f, HealthBarOffsetPosY));

	if (!DefaultWeaponClass.IsEmpty())
	{
		// 기본 무기 Array에서 2개까지만 서브 웨폰으로 지정
		// 0번은 Main 1번은 Sub
		if (!DefaultWeaponClass.IsEmpty())
		{
			int32 WeaponCount = 0;
			for (int i = 0; i < DefaultWeaponClass.Num(); ++i)
			{
				if (DefaultWeaponClass[i])
				{
					FActorSpawnParameters Params;
					Params.Owner = this;

					ASBWeapon* Weapon = GetWorld()->SpawnActor<ASBWeapon>(DefaultWeaponClass[i], GetActorTransform(), Params);
					
					Weapon->EquipItem((bool)WeaponCount);
					++WeaponCount;
				}

				if (WeaponCount > 1)
					break;
			}
		}
	}
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AMonsterCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage * 1000.f);
		GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Damaged : %f"), ActualDamage));
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		// 데미지 방향
		FVector ShotDirection = PointDamageEvent->ShotDirection;
		// 히트 위치 (표면 접촉 관점)
		FVector ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		// 히트 방향
		FVector ImpactDirection = PointDamageEvent->HitInfo.ImpactNormal;
		// 히트한 객체의 Location (객체 중심 관점)
		FVector HitLocation = PointDamageEvent->HitInfo.Location;

		ImpactEffect(ImpactPoint);

		HitReaction(EventInstigator->GetPawn());
	}

	return ActualDamage;
}

void AMonsterCharacter::OnDeath()
{
	//if (AAIController* AIController = Cast<AAIController>(GetController()))
	//{
	//	AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	//	AIController->UnPossess();
	//}

	//if (USkeletalMeshComponent* MeshComp = GetMesh())
	//{
	//	if (MeshComp->GetAnimInstance())
	//	{
	//		MeshComp->GetAnimInstance()->StopAllMontages(0.f);
	//		MeshComp->SetAnimInstanceClass(nullptr);
	//	}

	////	MeshComp->SetCollisionProfileName("Ragdoll");
	//MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	////	MeshComp->SetSimulatePhysics(true);
	////	MeshComp->WakeAllRigidBodies();

	////	FName BoneName = FName("Bip001-R-Calf");
	////	FVector BoneLocation = MeshComp->GetBoneTransform(BoneName).GetLocation();
	////	MeshComp->BreakConstraint(FVector::UpVector * 100.f, BoneLocation, BoneName);
	////	MeshComp->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
	//}

	////SliceMesh(FName("Bip001-Spine"));
	//SliceMesh(FName("Bip001-L-Calf"));
	////SliceMesh(FName("Bip001-R-Calf"));

	UProceduralMeshComponent* ProceduralMesh = CreateProceduralMeshFromBone(GetMesh(), FName("Bip001-L-Calf"));
}

UProceduralMeshComponent* AMonsterCharacter::CreateProceduralMeshFromBone(USkeletalMeshComponent* SkeletalMesh, const FName& BoneName)
{
	if (!SkeletalMesh || !SkeletalMesh->GetSkeletalMeshAsset())
	{
		return nullptr;
	}

	USkeletalMesh* SkeletalMeshAsset = SkeletalMesh->GetSkeletalMeshAsset();
	const int32 BoneIndex = SkeletalMesh->GetBoneIndex(BoneName);

	if (BoneIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bone '%s' not found!"), *BoneName.ToString());
		return nullptr;
	}

	// 버텍스 및 트라이앵글 데이터를 수집하기 위한 변수
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	// 본에 연결된 버텍스 수집
	const FSkeletalMeshRenderData* RenderData = SkeletalMeshAsset->GetResourceForRendering();
	const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[0];
	const FSkelMeshRenderSection& Section = LODData.RenderSections[0];

	for (uint32 VertexIndex = 0; VertexIndex < Section.NumVertices; ++VertexIndex)
	{
		const int32 MappedBoneIndex = LODData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 0);

		if (MappedBoneIndex == BoneIndex)
		{
			FVector VertexPosition = static_cast<FVector>(LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex));
			FVector Normal = static_cast<FVector>(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex));
			FVector2D UV = static_cast<FVector2D>(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0));

			Vertices.Add(VertexPosition);
			Normals.Add(Normal);
			UVs.Add(UV);
			Tangents.Add(FProcMeshTangent());
			VertexColors.Add(FColor(0, 0, 0, 255));
		}
	}

	TArray<int32> Triangles;

	const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODData.MultiSizeIndexContainer.GetIndexBuffer();
	// 삼각형 인덱스 데이터 생성 (단순 연결)
	const int32 NumIndices = IndexBuffer->Num();

	//메모리 미리 확보
	Triangles.SetNumUninitialized(NumIndices);
	for (int32 i = 0; i < NumIndices; i++)
	{
		//IndexBuffer Get(i) - 현재 처리 중인 삼각형을 구성하는 버텍스 인덱스를 가져옴.
		//결과적으로 Indices를 순환하면 3개씩 묶어서 삼각형을 그릴 수 있다.
		Triangles[i] = static_cast<int32>(IndexBuffer->Get(i));
	}


	// 프로시저럴 메시 생성
	UProceduralMeshComponent* ProcMesh = NewObject<UProceduralMeshComponent>(this);
	ProcMesh->RegisterComponent();
	ProcMesh->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepWorldTransform);

	ProcMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	//ProcMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

		//Convex Collision 추가
	if (Vertices.Num() > 0)
	{
		ProcMesh->ClearCollisionConvexMeshes();  // 기존 Collision 삭제
		//Convex Collision - 현재 Vertex 기반으로 Convex(볼록한) Collision 생성
		ProcMesh->AddCollisionConvexMesh(Vertices);  // Convex Collision 추가
	}

	ProcMesh->SetWorldLocation(GetMesh()->GetComponentLocation());
	ProcMesh->SetWorldRotation(GetMesh()->GetComponentRotation());

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ProcMesh->SetSimulatePhysics(true);
	ProcMesh->SetEnableGravity(true);

	UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
	if (SkeletalMeshMaterial)
	{
		ProcMesh->SetMaterial(0, SkeletalMeshMaterial);
		UE_LOG(LogTemp, Display, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
	}

	GetMesh()->SetVisibility(false);

	return ProcMesh;
}

void AMonsterCharacter::SliceMesh(FName BoneName)
{
	if (IsLocallyControlled())
	{
		GetInstigatorController()->UnPossess();
	}
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); // 필요에 따라 충돌 프로필 설정
	}

	// 1. 본 트랜스폼 가져오기
	FTransform BoneTransform = GetMesh()->GetBoneTransform(BoneName);

	//// 2. 본에 해당하는 프로시저럴 메시 생성
	UProceduralMeshComponent* ProceduralMesh = CreateProceduralMeshFromBone(GetMesh(), BoneName);
	if (!ProceduralMesh) return;

	// 3. SliceProceduralMesh로 절단
	FVector PlanePosition = BoneTransform.GetLocation();
	FVector PlaneNormal = FVector::UpVector;

	UProceduralMeshComponent* OtherHalfMesh = nullptr; // 절단 후 다른 절반을 받을 변수
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		ProceduralMesh,
		PlanePosition,
		PlaneNormal,
		true,
		OtherHalfMesh,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		nullptr
	);

	// 4. 절단된 메시를 본에 어태치
	ProceduralMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, BoneName);

	if (OtherHalfMesh)
	{
		FName ParentBoneName = GetMesh()->GetParentBone(BoneName);
		ProceduralMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, BoneName);
	}

	// 5. 본의 물리 제약 해제
	ProceduralMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 쿼리 및 물리 활성화

	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::UpVector, BoneName);
	GetMesh()->SetVisibility(false);
	ProceduralMesh->SetVisibility(true);

	ProceduralMesh->SetSimulatePhysics(true);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 쿼리 및 물리 활성화

	GetMesh()->SetSimulatePhysics(true);
	OtherHalfMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->WakeAllRigidBodies();      // 모든 물리 바디 깨우기

	GetMesh()->bBlendPhysics = true;

	GetMesh()->bReplicatePhysicsToAutonomousProxy = true;



	// 6. 임펄스 적용 (옵션)
	if (ProceduralMesh)
	{
		//ProceduralMesh->AddImpulse(FVector::UpVector * 3000000.0f, BoneName, true);
	}
}

void AMonsterCharacter::ImpactEffect(const FVector& Location)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Location);
	}
}

void AMonsterCharacter::HitReaction(const AActor* Attacker)
{
	check(CombatComponent)

	if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
	{
		PlayAnimMontage(HitReactAnimMontage);
	}
}

void AMonsterCharacter::OnTargeted(bool bTargeted)
{
	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetVisibility(bTargeted);
	}
}

bool AMonsterCharacter::CanBeTargeted()
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer TagCheck;
	TagCheck.AddTag(SBGameplayTags::Character_State_Death);
	return StateComponent->IsCurrentStateEqualToAny(TagCheck) == false;
}


void AMonsterCharacter::ActivateWeaponCollision(EWeaponType InWeaponType)
{
	if (CombatComponent)
	{

		switch (InWeaponType)
		{
		case EWeaponType::MainWeapon:
		{
			ASBWeapon* Weapon = CombatComponent->GetMainWeapon();
			if (Weapon)
				Weapon->ActivateCollision();
			break;
		}
		case EWeaponType::SubWeapon:
		{
			ASBWeapon* Weapon = CombatComponent->GetSubWeapon();
			if (Weapon)
				Weapon->ActivateCollision();
			break;
		}
		}
	}
}

void AMonsterCharacter::DeactivateWeaponCollision(EWeaponType InWeaponType)
{
	if (CombatComponent)
	{
		switch (InWeaponType)
		{
		case EWeaponType::MainWeapon:
		{
			ASBWeapon* Weapon = CombatComponent->GetMainWeapon();
			if (Weapon)
				Weapon->DeactivateCollision();
			break;
		}
		case EWeaponType::SubWeapon:
		{
			ASBWeapon* Weapon = CombatComponent->GetSubWeapon();
			if (Weapon)
				Weapon->DeactivateCollision();
			break;
		}
		}
	}
}

void AMonsterCharacter::PerformAttack(FGameplayTag& AttackTypeTag, FOnMontageEnded& MontageEndedDelegate)
{
	check(StateComponent)
	check(AttributeComponent)
	check(CombatComponent)

	if (const ASBWeapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(SBGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		AttributeComponent->ToggleStaminaRegeneration(false);

		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag))
		{
			if (UAnimInstance* AnimInstance = ACharacter::GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
			}
		}
	}
}

void AMonsterCharacter::FinishAttack()
{
	if(CombatComponent)
		CombatComponent->FinishAttack();
}

bool AMonsterCharacter::IsCombatEnabled()
{
	if (CombatComponent)
		return CombatComponent->IsCombatEnabled();

	return false;
}

void AMonsterCharacter::ToggleMonsterStateVisibility(bool bVisibility)
{
	if (MonsterStatBarWidget)
	{
		MonsterStatBarWidget->SetVisibility(bVisibility);
	}
}

ASBWeapon* AMonsterCharacter::GetMainWeapon()
{
	return CombatComponent->GetMainWeapon();
}

USBCombatComponent* AMonsterCharacter::GetCombatComponent()
{
	return CombatComponent;
}

void AMonsterCharacter::SetCombatEnabled(const bool bEnabled)
{
	CombatComponent->SetCombatEnabled(true);
}
