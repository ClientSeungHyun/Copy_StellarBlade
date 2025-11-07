// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestCharacter.h"
#include "TestCharacter.h"

#include "ProceduralMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ProceduralMeshComponent/Public/KismetProceduralMeshLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	//if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	//{
	//	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}

	//// Ragdoll
	//if (USkeletalMeshComponent* MeshComp = GetMesh())
	//{
	//	MeshComp->SetCollisionProfileName("Ragdoll");
	//	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//	MeshComp->SetSimulatePhysics(true);
	//}



	SelectVertices(0);
	ApplyVertexAlphaToSkeletalMesh();
	CopySkeletalMeshToProcedural(0);
	FVector SliceNormal = FVector(0, 0, 1); 
	SliceMeshAtBone(SliceNormal, true);

	//GetMesh()->SetVisibility(false);
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if UE_BUILD_DEVELOPMENT
	APlayerController* PC = GetWorld()->GetFirstPlayerController();


	static float debugoffsetPosX = 0.f;
	if (PC && PC->WasInputKeyJustPressed(EKeys::NumPadOne))
	{

		// 디버그용 코드 실행 (예: 메쉬 보정)
		FVector Center = GetAverageVertexPosition(FilteredVerticesArray);
		FVector ProcWorldCenter = ProcMeshComponent->GetComponentTransform().TransformPosition(Center);
		FVector ProcSocketWorld = GetMesh()->GetSocketLocation(ProceduralMeshAttachSocketName);

		FVector dd = ProcSocketWorld - ProcWorldCenter;
		ProcMeshComponent->AddWorldOffset(ProcSocketWorld - ProcWorldCenter);

		UWorld* World = GetWorld();
		if (World)
		{
			// 메시 중심 (파란색)
			DrawDebugSphere(World, ProcWorldCenter, 5.f, 12, FColor::Blue, false, 5.f);

			// 소켓 위치 (빨간색)
			DrawDebugSphere(World, ProcSocketWorld, 5.f, 12, FColor::Red, false, 5.f);

			// 두 점을 연결하는 선 (노란색)
			DrawDebugLine(World, ProcWorldCenter, ProcSocketWorld, FColor::Yellow, false, 5.f, 0, 1.0f);
		}
	}

	float Step = 1.f; // 한 번 누를 때 회전 변경 정도 (도 단위)
	static FRotator DebugOffsetRot = FRotator::ZeroRotator;

	// X축 (Pitch)
	if (PC->WasInputKeyJustPressed(EKeys::NumPadThree))
	{
		DebugOffsetRot.Pitch += Step;
	}
	if (PC->WasInputKeyJustPressed(EKeys::NumPadFour))
	{
		DebugOffsetRot.Pitch -= Step;
	}

	// Y축 (Yaw)
	if (PC->WasInputKeyJustPressed(EKeys::NumPadFive))
	{
		DebugOffsetRot.Yaw += Step;
	}
	if (PC->WasInputKeyJustPressed(EKeys::NumPadSix))
	{
		DebugOffsetRot.Yaw -= Step;
	}

	// Z축 (Roll)
	if (PC->WasInputKeyJustPressed(EKeys::NumPadSeven))
	{
		DebugOffsetRot.Roll += Step;
	}
	if (PC->WasInputKeyJustPressed(EKeys::NumPadEight))
	{
		DebugOffsetRot.Roll -= Step;
	}

	// 회전 적용
	if (DebugOffsetRot != FRotator::ZeroRotator)
	{
		ProcMeshComponent->SetRelativeRotation(DebugOffsetRot);
		UE_LOG(LogTemp, Log, TEXT("Debug Rot Offset -> Pitch: %.2f, Yaw: %.2f, Roll: %.2f"),
			DebugOffsetRot.Pitch, DebugOffsetRot.Yaw, DebugOffsetRot.Roll);
	}
	
#endif
}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestCharacter::SelectVertices(int32 LODIndex)
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
	Indices.SetNum(NumIndices); // 모든 값을 0으로 초기화하며 메모리 공간 확보보
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

void ATestCharacter::ApplyVertexAlphaToSkeletalMesh()
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

void ATestCharacter::CopySkeletalMeshToProcedural(int32 LODIndex)
{
	if (!GetMesh() || !ProcMeshComponent) {
		UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
		return;
	}

	//Skeletal Mesh의 Location과 Rotation을 들고온다.
	FVector MeshLocation = GetMesh()->GetComponentLocation();
	FRotator MeshRotation = GetMesh()->GetComponentRotation();

	//Skeletal Mesh의 Location과 Rotation을 Procedural Mesh에 적용한다.
	ProcMeshComponent->SetWorldLocation(MeshLocation) ;
	ProcMeshComponent->SetWorldRotation(MeshRotation);

	//Section Index - 어떤 Section부터 시작하는가?, Vertices - 어떤 vertex를 사용하는가?
	//Indices - 어떤 삼각형 구조를 사용하는가?, Normals, UV, Colors, Tangents, bCreateCollision - 충돌 활성화

	ProcMeshComponent->CreateMeshSection(0, FilteredVerticesArray, Indices, Normals, UV, VertexColors, Tangents, true);
	UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
	if (SkeletalMeshMaterial) {
		ProcMeshComponent->SetMaterial(0, SkeletalMeshMaterial);
		//UE_LOG(LogTemp, Display, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
	}
	else UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
}

void ATestCharacter::SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf)
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

	UProceduralMeshComponent* OtherHalfMesh = nullptr;		//잘린 Procedural Mesh가 OtherHalfMesh가 된다.
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		ProcMeshComponent,
		BoneWorldLocation,
		SliceNormal,
		bCreateOtherHalf,
		OtherHalfMesh,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		CapMaterial                           //절단면 Material
	);

	if (!OtherHalfMesh) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Failed to slice mesh at bone '%s'."), *TargetBoneName.ToString());
		return;
	}
	if (ProceduralMeshAttachSocketName.IsNone() || OtherHalfMeshAttachSocketName.IsNone()) {
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: One or both Socket Names are invalid!"));
		return;
	}

	ProcMeshComponent->SetSimulatePhysics(false);
	OtherHalfMesh->SetSimulatePhysics(false);

	//Procedural Mesh를 특정 Socket에 Attach
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ProcMeshComponent->AttachToComponent(GetMesh(), TransformRules, ProceduralMeshAttachSocketName);
	OtherHalfMesh->AttachToComponent(GetMesh(), TransformRules, OtherHalfMeshAttachSocketName);

	OtherHalfMesh->SetVisibility(false);


	// Attach with SnapToTarget
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	ProcMeshComponent->AttachToComponent(GetMesh(), AttachRules, ProceduralMeshAttachSocketName);
	OtherHalfMesh->AttachToComponent(GetMesh(), AttachRules, OtherHalfMeshAttachSocketName);


	OtherHalfMesh->AddLocalRotation(OtherHalfRotation);

	// 위치 오프셋 보정 (중심 → 소켓)
	FVector Center = GetAverageVertexPosition(FilteredVerticesArray);
	FVector ProcWorldCenter = ProcMeshComponent->GetComponentTransform().TransformPosition(Center);
	FVector ProcSocketWorld = GetMesh()->GetSocketLocation(ProceduralMeshAttachSocketName);
	//ProcMeshComponent->AddWorldOffset(ProcSocketWorld - ProcWorldCenter);

	//FProcMeshSection* OtherSection = OtherHalfMesh->GetProcMeshSection(0);
	//TArray<FVector> OtherVertices;
	//for (const FProcMeshVertex& V : OtherSection->ProcVertexBuffer)
	//	OtherVertices.Add(V.Position);

	//FVector OtherCenter = GetAverageVertexPosition(OtherVertices);
	//FVector OtherWorldCenter = OtherHalfMesh->GetComponentTransform().TransformPosition(OtherCenter);
	//FVector OtherSocketWorld = GetMesh()->GetSocketLocation(OtherHalfMeshAttachSocketName);
	//OtherHalfMesh->AddWorldOffset(OtherSocketWorld - OtherWorldCenter);




	//Ragdoll 적용 & Bone 자름.
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->BreakConstraint(FVector(1000.f, 1000.f, 1000.f), FVector::ZeroVector, TargetBoneName);
	GetMesh()->SetSimulatePhysics(true);

	//Procedural Mesh에 물리 적용
	//ProcMeshComponent->SetSimulatePhysics(true); //-> true 시 따로 움직인다.
	ProcMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

FVector ATestCharacter::GetAverageVertexPosition(const TArray<FVector>& Vertices)
{
	if (Vertices.Num() == 0) return FVector::ZeroVector;

	FVector Sum = FVector::ZeroVector;
	for (const FVector& V : Vertices)
		Sum += V;

	return Sum / Vertices.Num();
}


//void ATestCharacter::CreateProceduralMesh()
//{
	//USkeletalMeshComponent* SkeletalMesh = GetMesh();

	//if (!SkeletalMesh || !SkeletalMesh->GetSkeletalMeshAsset())
	//{
	//	return;
	//}

	//USkeletalMesh* SkeletalMeshAsset = SkeletalMesh->GetSkeletalMeshAsset();
	////const int32 BoneIndex = SkeletalMesh->GetBoneIndex(BoneName);

	////if (BoneIndex == INDEX_NONE)
	////{
	////	UE_LOG(LogTemp, Warning, TEXT("Bone '%s' not found!"), *BoneName.ToString());
	////	return nullptr;
	////}

	//// 버텍스 및 트라이앵글 데이터를 수집하기 위한 변수
	//TArray<FVector> Vertices;
	//TArray<FVector> Normals;
	//TArray<FVector2D> UVs;
	//TArray<FLinearColor> VertexColors;
	//TArray<FProcMeshTangent> Tangents;

	//// 본에 연결된 버텍스 수집
	//const FSkeletalMeshRenderData* RenderData = SkeletalMeshAsset->GetResourceForRendering();
	//const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[0];
	//const FSkelMeshRenderSection& Section = LODData.RenderSections[0];

	//for (uint32 VertexIndex = 0; VertexIndex < Section.NumVertices; ++VertexIndex)
	//{
	//	const int32 MappedBoneIndex = LODData.SkinWeightVertexBuffer.GetBoneIndex(VertexIndex, 0);

	//	FVector VertexPosition = static_cast<FVector>(LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex));
	//	FVector Normal = static_cast<FVector>(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex));
	//	FVector2D UV = static_cast<FVector2D>(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0));

	//	Vertices.Add(VertexPosition);
	//	Normals.Add(Normal);
	//	UVs.Add(UV);
	//	Tangents.Add(FProcMeshTangent());
	//	VertexColors.Add(FColor(0, 0, 0, 255));
	//}

	//TArray<int32> Triangles;

	//const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODData.MultiSizeIndexContainer.GetIndexBuffer();
	//// 삼각형 인덱스 데이터 생성 (단순 연결)
	//const int32 NumIndices = IndexBuffer->Num();

	////메모리 미리 확보
	//Triangles.SetNumUninitialized(NumIndices);
	//for (int32 i = 0; i < NumIndices; i++)
	//{
	//	//IndexBuffer Get(i) - 현재 처리 중인 삼각형을 구성하는 버텍스 인덱스를 가져옴.
	//	//결과적으로 Indices를 순환하면 3개씩 묶어서 삼각형을 그릴 수 있다.
	//	Triangles[i] = static_cast<int32>(IndexBuffer->Get(i));
	//}


	//// 프로시저럴 메시 생성
	//ProceduralMesh = NewObject<UProceduralMeshComponent>(this);
	//ProceduralMesh->RegisterComponent();
	//ProceduralMesh->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepWorldTransform);

	//ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	////ProcMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

	//	//Convex Collision 추가
	//if (Vertices.Num() > 0)
	//{
	//	ProceduralMesh->ClearCollisionConvexMeshes();  // 기존 Collision 삭제
	//	//Convex Collision - 현재 Vertex 기반으로 Convex(볼록한) Collision 생성
	//	ProceduralMesh->AddCollisionConvexMesh(Vertices);  // Convex Collision 추가
	//}

	//ProceduralMesh->SetWorldLocation(GetMesh()->GetComponentLocation());
	//ProceduralMesh->SetWorldRotation(GetMesh()->GetComponentRotation());

	//ProceduralMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//ProceduralMesh->SetCollisionObjectType(ECC_WorldDynamic);
	//ProceduralMesh->SetSimulatePhysics(true);
	//ProceduralMesh->SetEnableGravity(true);

	//UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
	//if (SkeletalMeshMaterial)
	//{
	//	ProceduralMesh->SetMaterial(0, SkeletalMeshMaterial);
	//	UE_LOG(LogTemp, Display, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
	//}

	//GetMesh()->SetVisibility(false);
//}

