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
#include "AI/MonsterAIController.h"
#include "Player/SBEveWeapon.h"

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
	LockOnWidgetComponent->SetDrawSize(FVector2D(15.f, 15.f));
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

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(GetRootComponent());

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

	if (LockOnWidgetComponent)
	{
		LockOnWidgetComponent->SetRelativeLocation(LockOnWidgetPositionOffset);
	}

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

	if (DissolveCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("UpdateDissolveProgress"));
		DissolveTimeline.AddInterpFloat(DissolveCurveFloat, TimelineProgress);
	}


	InitDynamicMaterials();

	if(bIsSliceObject)
		SelectVertices(0);
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DissolveTimeline.TickTimeline(DeltaTime);
}

float AMonsterCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float  ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AttributeComponent)
	{
		AttributeComponent->TakeDamageAmount(ActualDamage);
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

		ASBEveWeapon* PlayerWeapon = Cast<ASBEveWeapon>(DamageCauser);

		FVector HitDirection = PlayerWeapon->GetAttackDirection();

		//Vertex Shake
		if (HitDirection != FVector::ZeroVector)
		{
			const FVector AttackDir = PlayerWeapon->GetAttackDirection();
			const float CurrentTime = GetWorld()->GetTimeSeconds();

			for (const FDynamicMaterialInfo& Info : DynamicMaterials)
			{
				if (Info.DynamicMaterial)
				{
					Info.DynamicMaterial->SetVectorParameterValue("HitWorldPosition", ImpactPoint);
					Info.DynamicMaterial->SetVectorParameterValue("HitDirection", AttackDir);
					Info.DynamicMaterial->SetScalarParameterValue("HitStrength", 1.0f);
					Info.DynamicMaterial->SetScalarParameterValue("HitTime", CurrentTime);
				}
			}
		}
	}

	return ActualDamage;
}

void AMonsterCharacter::OnDeath()
{
	if (bIsDead)
		return;

	bIsDead = true;
	MonsterStatBarWidget->DestroyComponent();

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
		AIController->UnPossess();
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (MeshComp->GetAnimInstance())
		{
			MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
			MeshComp->GetAnimInstance()->StopAllMontages(0.f);
			MeshComp->SetAnimInstanceClass(nullptr);
		}
	}

	if (CombatComponent)
		CombatComponent->SetWeapon(NULL, false);

	if (bIsSliceObject)
	{
		ApplyVertexAlphaToSkeletalMesh();
		CopySkeletalMeshToProcedural(0);
		FVector SliceNormal = FVector(0, 0, 1);
		SliceMeshAtBone(SliceNormal, true);
	}
	else
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DeadTimerHandle,
		this,
		&AMonsterCharacter::StartDissolve,
		DissolveDelayTime,
		false);
}

void AMonsterCharacter::ImpactEffect(const FVector& Location)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Location);
	}

	if (ImpactNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactNiagara,
			Location,
			FRotator::ZeroRotator,
			FVector(0.1f, 0.1f, 0.1f),
			true,   // bAutoDestroy
			true    // bAutoActivate
		);
	}

}

void AMonsterCharacter::HitReaction(const AActor* Attacker)
{
	check(CombatComponent)

	AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(GetController());
	if (!MonsterAIController)
		return;

	UBrainComponent* Brain = MonsterAIController->GetBrainComponent();
	if (!Brain)
		return;

	if (CombatComponent->GetMainWeapon())
	{
		if (UAnimMontage* HitReactAnimMontage = CombatComponent->GetMainWeapon()->GetHitReactMontage(Attacker))
		{
			Brain->StopLogic(TEXT("Hit"));
			PlayAnimMontage(HitReactAnimMontage);

			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
					{
						AMonsterAIController* MonsterAIController = Cast<AMonsterAIController>(GetController());
						if (!MonsterAIController)
							return;

						UBrainComponent* Brain = MonsterAIController->GetBrainComponent();
						if (!Brain)
							return;

						const float DelayTime = 0.6f;
						FTimerHandle TimerHandle;
						if (UWorld* World = GetWorld())
						{
							World->GetTimerManager().SetTimer(
								TimerHandle,
								[Brain]()
								{
									Brain->StartLogic();
								},
								DelayTime,
								false
							);
						}
					});

				AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactAnimMontage);
			}
		}
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

	if (MonsterGrowlSounds.Num() > 0)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();

		int32 RandomIndex = FMath::RandRange(0, MonsterGrowlSounds.Num() - 1);
		USoundBase* SelectedSound = nullptr;
		if (MonsterGrowlSounds.IsValidIndex(RandomIndex))
			SelectedSound = MonsterGrowlSounds[RandomIndex];

		if(SelectedSound)
			UGameplayStatics::PlaySoundAtLocation(SkeletalMeshComponent->GetWorld(), SelectedSound, SkeletalMeshComponent->GetComponentLocation());
	}

	if (const ASBWeapon* Weapon = CombatComponent->GetMainWeapon())
	{
		StateComponent->SetState(SBGameplayTags::Character_State_Attacking);
		CombatComponent->SetLastAttackType(AttackTypeTag);
		AttributeComponent->ToggleStaminaRegeneration(false);

		if (UAnimMontage* Montage = Weapon->GetRandomMontageForTag(AttackTypeTag))
		{
			if (UAnimInstance* AnimInstance = ACharacter::GetMesh()->GetAnimInstance())
			{
				UKismetSystemLibrary::PrintString(
					this,
					FString::Printf(TEXT("%s 공격이 실행됨"), *AttackTypeTag.ToString()),  // 문자열 포맷
					true,  
					true,  
					FLinearColor::Green,
					2.0f    
				);

				if (AttackTypeTag.MatchesTag(SBGameplayTags::Monster_Attack_Blink))
				{
					ResetAttackCount();
				}

				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);

				if(Weapon->IsHaveBlinkAttack())
					++CurrentAttackCount;
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

void AMonsterCharacter::SelectVertices(int32 LODIndex)
{
	if (!GetMesh() || !ProcMeshComponent) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
		return;
	}

	// SkeletalMesh를 가져온다.
	USkeletalMesh* SkeletalMesh = GetMesh()->GetSkeletalMeshAsset();
	if (!SkeletalMesh) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMesh is null."));
		return;
	}

	//GetResourceForRendering - Skeletal Mesh의 렌더링 데이터를 가져오는 함수
	const FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
	if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex)) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: LODRenderData[%d] is not valid."), LODIndex);
		return;
	}
	//vertex의 총 개수를 들고온다.
	NumVertices = RenderData->LODRenderData[LODIndex].GetNumVertices();

	//SkeletalMesh에서 LODRenderData를 가져온다.LODRenderData는 버텍스 데이터, 인덱스 데이터, 섹션 정보 등이 포함
	//FSkeletalMeshLODRenderData란 LOD의 Mesh 데이터를 가지고 있는 구조체이다.
	const FSkeletalMeshLODRenderData& LODRenderData = RenderData->LODRenderData[LODIndex];
	//SkinWeightVertexBuffer를 가져온다. -> vertex가 어떤 Bone에 영향을 받는지 저장하는 데이터이며 Animation에서 사용 예정
	const FSkinWeightVertexBuffer& SkinWeights = LODRenderData.SkinWeightVertexBuffer;

	//위치를 들고온다.
	FTransform MeshTransform = GetMesh()->GetComponentTransform();
	FVector TargetBoneLocation = GetMesh()->GetBoneLocation(TargetBoneName);

	FVector BoneLocalPos = GetMesh()->GetBoneLocation(TargetBoneName, EBoneSpaces::ComponentSpace);

	int32 TargetBoneIndex = GetMesh()->GetBoneIndex(TargetBoneName);
	if (TargetBoneIndex == INDEX_NONE) {
		UE_LOG(LogTemp, Warning, TEXT("Target bone not found: %s"), *TargetBoneName.ToString());
		return;
	}

	int32 vertexCounter = 0;
	for (const FSkelMeshRenderSection& Section : LODRenderData.RenderSections) {
		//NumVertices - 해당 Section의 Vertex 수, BaseVertexIndex - 해당 Section의 시작 Vertex Index
		const int32 NumSourceVertices = Section.NumVertices;
		const int32 BaseVertexIndex = Section.BaseVertexIndex;

		for (int32 i = 0; i < NumSourceVertices; i++) {
			const int32 VertexIndex = i + BaseVertexIndex;

			//vertex의 위치를 가져온다. -> LODRenderData.StaticVertexBuffers.PositionVertexBuffer(현재 LOD의 Vertex 위치 데이터를 저장하는 버퍼)
			//.VertexPosition(VertexIndex)-> VertexIndex의 위치를 가져온다. 반환 타입이 FVector3f이다.
			const FVector3f SkinnedVectorPos = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);		//로컬 좌표 반환
			FVector WorldVertexPosition = MeshTransform.TransformPosition(FVector(SkinnedVectorPos)); // FVector3f -> FVector 변환 & 로컬 -> 월드 좌표
			//UE_LOG(LogTemp, Display, TEXT("WorldVertexPosition : %s"), *WorldVertexPosition.ToString());
			float DistanceToBone = FVector::Dist(WorldVertexPosition, TargetBoneLocation);

			////TargetBoneLocation을 기준으로 일정 거리 내에 있는 Vertex만 추가해서 Procedural Mesh 생성
			if (DistanceToBone <= CreateProceduralMeshDistance)
			{
				// 위치 정보 추가
				FVector LocalVertexPosition = FVector(SkinnedVectorPos);
				VertexIndexMap.Add(VertexIndex, vertexCounter);
				//WorldVertexPosition을 사용하면 다른 위치에 Procedural Mesh가 생성된다.
				FilteredVerticesArray.Add(LocalVertexPosition);
				vertexCounter += 1;
				// 노멀(Normal), 탄젠트(Tangent) 추가
				const FVector3f Normal = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex);
				const FVector3f TangentX = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);
				const FVector2f SourceUVs = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
				Normals.Add(FVector(Normal));
				Tangents.Add(FProcMeshTangent(FVector(TangentX), false));
				UV.Add(FVector2D(SourceUVs));

				VertexColors.Add(FColor(0, 0, 0, 255));
			}
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("VertexIndexMan Count : %d"), VertexIndexMap.Num());
	//LODRenderData.MultiSizeIndexContainer.GetIndexBuffer()는 원래 Skeletal Mesh의 각 vertex가 어떻게 삼각형으로 구성되어있었는지를 들고온다.
	const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODRenderData.MultiSizeIndexContainer.GetIndexBuffer();
	if (!IndexBuffer) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: Index buffer is null."));
		return;
	}

	//현재 LOD의 총 Index 수를 가져온다.
	const int32 NumIndices = IndexBuffer->Num();
	Indices.SetNum(NumIndices);
	for (int32 i = 0; i < NumIndices; i += 3) {
		//IndexBuffer Get(i) - 현재 처리 중인 삼각형을 구성하는 버텍스 인덱스를 가져옴.
		//VertexIndex : Get(0) = a, Get(1) = b, Get(2) = c로 abc삼각형, Get(3) = c, Get(4) = d, Get(5) = a로 cda삼각형 (여기서 abcd는 FVector위치라고 취급)
		//즉, 첫 BaseVertexIndex에서 그려지는 삼각형부터 순서대로 삼각형이 그려지는 vertex의 Index를 가져온다.
		//결과적으로 Indices를 순환하면 3개씩 묶어서 삼각형을 그릴 수 있다.
		//uint32가 반환되어 int32로 Casting, 데이터 일관성을 위해 Casting한다.
		//Indices[i] = static_cast<int32>(IndexBuffer->Get(i));
		//아래 코드는 3각형을 이루는 3개의 i를 들고오는 것이다.
		int32 OldIndex0 = static_cast<int32>(IndexBuffer->Get(i));
		int32 OldIndex1 = static_cast<int32>(IndexBuffer->Get(i + 1));
		int32 OldIndex2 = static_cast<int32>(IndexBuffer->Get(i + 2));

		int32 NewIndex0 = VertexIndexMap.Contains(OldIndex0) ? VertexIndexMap[OldIndex0] : -1;
		int32 NewIndex1 = VertexIndexMap.Contains(OldIndex1) ? VertexIndexMap[OldIndex1] : -1;
		int32 NewIndex2 = VertexIndexMap.Contains(OldIndex2) ? VertexIndexMap[OldIndex2] : -1;
		// 기존 VertexIndex가 NewVertexIndex에 포함된 경우만 추가 - 실제로 내가 vertex 수집한 곳에 있는 Index인지 확인한다.
		//NewIndex >= FilteredVerticesArray.Num() - VertexIndexMap이 잘못된 값을 반환하거나, Indices 배열에 유효하지 않은 인덱스가 추가될 때 발생
		if (NewIndex0 < 0 || NewIndex0 >= FilteredVerticesArray.Num() || NewIndex1 < 0 || NewIndex1 >= FilteredVerticesArray.Num() || NewIndex2 < 0 || NewIndex2 >= FilteredVerticesArray.Num()) {
			//UE_LOG(LogTemp, Warning, TEXT("Skipping triangle due to invalid indices: (%d, %d, %d) → (%d, %d, %d)"),	OldIndex0, OldIndex1, OldIndex2, NewIndex0, NewIndex1, NewIndex2);
		}
		else {
			Indices.Add(NewIndex0);
			Indices.Add(NewIndex1);
			Indices.Add(NewIndex2);
		}
	}

	//
	// for (int32 i = 0; i < FilteredVerticesArray.Num(); i++) {
	// 	// 잘린 부분의 버텍스를 Black(0, 0, 0)으로 설정
	// 	VertexColors.Add(FColor(0, 0, 0, 0));  // 알파 값 포함
	// }
}

void AMonsterCharacter::ApplyVertexAlphaToSkeletalMesh()
{
	if (!GetMesh() || !GetMesh()->GetSkeletalMeshAsset()) return;

	TArray<FLinearColor> LinearVertexColors;
	LinearVertexColors.Init(FLinearColor(1, 1, 1, 1), NumVertices); // 흰색(보임)

	// VertexIndexMap을 활용해 잘린 부분만 색상을 변경
	for (const TPair<int32, int32>& Pair : VertexIndexMap) {
		int32 ColorChangeIndex = Pair.Key;  // 원본 Skeletal Mesh의 버텍스 인덱스
		if (ColorChangeIndex >= 0) {		//잘못된 Index 방지.
			LinearVertexColors[ColorChangeIndex] = FLinearColor(0, 0, 0, 0);  // 검은색 = 마스킹 처리
		}
	}

	// Skeletal Mesh에 버텍스 컬러 적용
	GetMesh()->SetVertexColorOverride_LinearColor(0, LinearVertexColors);
	GetMesh()->MarkRenderStateDirty(); // 렌더 상태 갱신
}

void AMonsterCharacter::CopySkeletalMeshToProcedural(int32 LODIndex)
{
	if (!GetMesh() || !ProcMeshComponent) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
		return;
	}

	//Skeletal Mesh의 Location과 Rotation을 들고온다.
	FVector MeshLocation = GetMesh()->GetComponentLocation();
	FRotator MeshRotation = GetMesh()->GetComponentRotation();

	//Skeletal Mesh의 Location과 Rotation을 Procedural Mesh에 적용한다.
	ProcMeshComponent->SetWorldLocation(MeshLocation);
	ProcMeshComponent->SetWorldRotation(MeshRotation);

	//Section Index - 어떤 Section부터 시작하는가?, Vertices - 어떤 vertex를 사용하는가?
	//Indices - 어떤 삼각형 구조를 사용하는가?, Normals, UV, Colors, Tangents, bCreateCollision - 충돌 활성화
	ProcMeshComponent->CreateMeshSection(0, FilteredVerticesArray, Indices, Normals, UV, VertexColors, Tangents, true);
	UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
	if (SkeletalMeshMaterial) {
		ProcMeshComponent->SetMaterial(0, SkeletalMeshMaterial);
	}
	else UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));

	ProcMeshComponent->SetWorldScale3D(GetMesh()->GetComponentScale());
}

void AMonsterCharacter::SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf)
{
	if (!GetMesh() || !ProcMeshComponent) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: SkeletalMeshComponent or ProcMeshComponent is null."));
		return;
	}

	const FVector BoneWorldLocation = GetMesh()->GetBoneLocation(TargetBoneName, EBoneSpaces::WorldSpace);
	FVector BoneLocation = GetMesh()->GetBoneLocation(TargetBoneName);
	if (BoneLocation == FVector::ZeroVector) {
		UE_LOG(LogTemp, Error, TEXT("SliceMeshAtBone: Failed to get Bone '%s' location. Check if the bone exists in the skeleton."), *TargetBoneName.ToString());
		return;
	}

	UMaterialInterface* ProcMeshMaterial = ProcMeshComponent->GetMaterial(0);
	if (!ProcMeshMaterial) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Procedural mesh has no material assigned."));
	}

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		ProcMeshComponent,
		BoneWorldLocation,
		SliceNormal,
		bCreateOtherHalf,
		OtherMeshComponent,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		CapMaterial                           //절단면 Material
	);

	if (!OtherMeshComponent) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Failed to slice mesh at bone '%s'."), *TargetBoneName.ToString());
		return;
	}
	if (ProceduralMeshAttachSocketName.IsNone() || OtherHalfMeshAttachSocketName.IsNone()) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: One or both Socket Names are invalid!"));
		return;
	}

	ProcMeshComponent->SetSimulatePhysics(false);
	OtherMeshComponent->SetSimulatePhysics(false);

	//Procedural Mesh를 특정 Socket에 Attach
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ProcMeshComponent->AttachToComponent(GetMesh(), TransformRules, ProceduralMeshAttachSocketName);
	OtherMeshComponent->AttachToComponent(GetMesh(), TransformRules, OtherHalfMeshAttachSocketName);

	// 보정 회전 적용
	FRotator ProcSocketRot = GetMesh()->GetSocketTransform(ProceduralMeshAttachSocketName, RTS_Component).Rotator();
	FRotator OtherSocketRot = GetMesh()->GetSocketTransform(OtherHalfMeshAttachSocketName, RTS_Component).Rotator();

	FVector Center = GetAverageVertexPosition(FilteredVerticesArray);
	FVector ProcWorldCenter = ProcMeshComponent->GetComponentTransform().TransformPosition(Center);
	FVector ProcSocketWorld = GetMesh()->GetSocketLocation(ProceduralMeshAttachSocketName);
	ProcMeshComponent->AddWorldOffset(ProcSocketWorld - ProcWorldCenter);

	FProcMeshSection* OtherSection = OtherMeshComponent->GetProcMeshSection(0);
	TArray<FVector> OtherVertices;
	for (const FProcMeshVertex& V : OtherSection->ProcVertexBuffer)
		OtherVertices.Add(V.Position);

	FVector OtherCenter = GetAverageVertexPosition(OtherVertices);
	FVector OtherWorldCenter = OtherMeshComponent->GetComponentTransform().TransformPosition(OtherCenter);
	FVector OtherSocketWorld = GetMesh()->GetSocketLocation(OtherHalfMeshAttachSocketName);
	OtherMeshComponent->AddWorldOffset(OtherSocketWorld - OtherWorldCenter);


	//Ragdoll 적용 & Bone 자름.
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(TargetBoneName, true, true);
	GetMesh()->BreakConstraint(FVector(1000.f, 1000.f, 1000.f), FVector::ZeroVector, TargetBoneName);
	GetMesh()->SetSimulatePhysics(true);

	ProcMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OtherMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMonsterCharacter::InitDynamicMaterials()
{
	TArray<USkeletalMeshComponent*> MeshComponents;
	MeshComponents.Add(GetMesh());

	TArray<USceneComponent*> ChildrenComp;
	GetMesh()->GetChildrenComponents(true, ChildrenComp);

	for (USceneComponent* Ch : ChildrenComp)
	{
		if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(Ch))
		{
			MeshComponents.Add(SkelComp);
		}
	}

	for (USkeletalMeshComponent* MeshComp : MeshComponents)
	{
		int32 MaterialCount = MeshComp->GetNumMaterials();

		for (int32 Index = 0; Index < MaterialCount; ++Index)
		{
			UMaterialInterface* Mat = MeshComp->GetMaterial(Index);
			if (Mat == nullptr)
				continue;

			UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Mat, this);
			if (MID)
			{
				MeshComp->SetMaterial(Index, MID);

				// 필요하다면 나중에 업데이트 위해 저장
				DynamicMaterials.Add(FDynamicMaterialInfo{ MeshComp, Index, MID });
			}
		}
	}
}

void AMonsterCharacter::UpdateDissolveProgress(const float InValue)
{
	// 저장된 모든 Dynamic Material 에 값 적용
	for (const FDynamicMaterialInfo& Info : DynamicMaterials)
	{
		if (Info.DynamicMaterial)
		{
			Info.DynamicMaterial->SetScalarParameterValue("DissolveParam", InValue);
		}
	}

	// Dissolve 완료 시 삭제 로직
	if (InValue >= 1.f)
	{
		// AI 정지
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			if (AI->BrainComponent)
			{
				AI->BrainComponent->StopLogic(TEXT("Enemy Died"));
			}

			AI->UnPossess();
			AI->Destroy();
		}

		// 몬스터 삭제
		Destroy();
	}
}

void AMonsterCharacter::StartDissolve()
{
	DissolveTimeline.PlayFromStart();
	SetActorTickEnabled(true);
}

FVector AMonsterCharacter::GetAverageVertexPosition(const TArray<FVector>& Vertices)
{
	if (Vertices.Num() == 0) return FVector::ZeroVector;

	FVector Sum = FVector::ZeroVector;
	for (const FVector& V : Vertices)
		Sum += V;

	return Sum / Vertices.Num();
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
	CombatComponent->SetCombatEnabled(bEnabled);
}

bool AMonsterCharacter::IsHaveBlinkAttack() const
{
	return CombatComponent->GetMainWeapon()->IsHaveBlinkAttack();
}

