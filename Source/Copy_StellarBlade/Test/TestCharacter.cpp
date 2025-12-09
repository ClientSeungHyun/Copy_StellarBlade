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

	OtherMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("OtherMesh"));
	OtherMeshComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();


	SelectVertices(0);


	//GetMesh()->SetVisibility(false);
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if UE_BUILD_DEVELOPMENT
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (PC->WasInputKeyJustPressed(EKeys::NumPadFour))
	{
		ApplyVertexAlphaToSkeletalMesh();
		CopySkeletalMeshToProcedural(0);
		FVector SliceNormal = FVector(0, 0, 1);
		SliceMeshAtBone(SliceNormal, true);
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
	}
	else UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));

	ProcMeshComponent->SetWorldScale3D(GetMesh()->GetComponentScale());
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

FVector ATestCharacter::GetAverageVertexPosition(const TArray<FVector>& Vertices)
{
	if (Vertices.Num() == 0) return FVector::ZeroVector;

	FVector Sum = FVector::ZeroVector;
	for (const FVector& V : Vertices)
		Sum += V;

	return Sum / Vertices.Num();
}

//void ATestCharacter::SliceMesh()
//{
//	TRACE_CPUPROFILER_EVENT_SCOPE(SliceMesh_Sync);
//	check(NumVertices != 0);
//
//	USkeletalMeshComponent* OriginMeshComp = GetMesh();
//	FVector TargetBoneLocation = OriginMeshComp->GetBoneLocation(TargetBoneName);
//
//	FPlane SlicePlane = FPlane(TargetBoneLocation, FVector::UpVector);
//
//	if (!OriginMeshComp->IsVisible())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Invisible meshes cannot be sliced."));
//		return;
//	}
//
//	TSet<uint32> PSideVertices;
//	TSet<uint32> NSideVertices;
//	TSet<uint32> IntersectingVertices;
//	SplitVerticesByPlane(SlicePlane, PSideVertices, NSideVertices, IntersectingVertices);
//	if (PSideVertices.IsEmpty() || NSideVertices.IsEmpty() || IntersectingVertices.IsEmpty())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("The plane and the mesh do not intersect."));
//		return;
//	}
//
//
//	USkeletalMesh* PSideSkeletalMesh = BuildVertexToSkeletalMesh(PSideVertices, FName(TEXT("PSide")));
//	USkeletalMesh* NSideSkeletalMesh = BuildVertexToSkeletalMesh(NSideVertices, FName(TEXT("NSide")));
//	//UAPSkinnedProceduralMeshComponent* ProceduralMeshComp = BuildVertexToProceduralMeshComp(IntersectingVertices);
//}
//
//USkeletalMesh* ATestCharacter::BuildVertexToSkeletalMesh(const TSet<uint32>& Indices, const FName& AssetNameTag)
//{
//	USkeletalMesh* OriginalMesh = GetMesh()->GetSkeletalMeshAsset();
//	FName NewAssetName = MakeUniqueObjectName(this, USkeletalMesh::StaticClass(),
//		*FString::Printf(TEXT("%s_%s"), *OriginalMesh->GetName(), *AssetNameTag.ToString()));
//
//	USkeletalMesh* NewSkeletalMesh = NewObject<USkeletalMesh>(this, NewAssetName);
//	//APSkeletalMeshBuilder Builder(NewSkeletalMesh, OriginalMesh, Indices);
//
//	//if (Builder.BuildSkeletalMesh())
//	//{
//	//	return NewSkeletalMesh;
//	//}
//
//	return nullptr;
//}
//
//void ATestCharacter::SplitVerticesByPlane(const FPlane& SlicePlane,
//	TSet<uint32>& OutPositiveVertexIndices,
//	TSet<uint32>& OutNegativeVertexIndices,
//	TSet<uint32>& OutIntersectingVertexIndices) const
//{
//	TRACE_CPUPROFILER_EVENT_SCOPE(SliceMesh_Split);
//	OutPositiveVertexIndices.Empty();
//	OutNegativeVertexIndices.Empty();
//	OutIntersectingVertexIndices.Empty();
//
//	TArray<FFinalSkinVertex> SkinnedVertices;
//	GetCPUSkinnedVertices(SkinnedVertices, 0);
//	if (SkinnedVertices.Num() != NumVertices)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[SplitVerticesByPlane] SkinnedVertices size mismatch! Expected: %d, Actual: %d"), NumVertices, SkinnedVertices.Num());
//		return;
//	}
//
//	const FPlane LocalSlicePlane = SlicePlane.TransformBy(GetComponentTransform().Inverse().ToMatrixWithScale());
//
//	TArray<float> Distances;
//	Distances.SetNumUninitialized(NumVertices);
//
//	ParallelFor(NumVertices, [&](int32 VertexIndex)
//		{
//			Distances[VertexIndex] = LocalSlicePlane.PlaneDot(FVector(SkinnedVertices[VertexIndex].Position));
//		});
//
//	const FSkeletalMeshLODRenderData& LODData = GetSkeletalMeshRenderData()->LODRenderData[0];
//	TArray<uint32> IndexBuffer;
//	LODData.MultiSizeIndexContainer.GetIndexBuffer(IndexBuffer);
//
//	FCriticalSection IntersectingSetLock;
//
//	const int32 NumTriangles = IndexBuffer.Num() / 3;
//
//	ParallelFor(NumTriangles, [&](int32 TriIndex)
//		{
//			const int32 BaseIndex = TriIndex * 3;
//			const uint32 I0 = IndexBuffer[BaseIndex + 0];
//			const uint32 I1 = IndexBuffer[BaseIndex + 1];
//			const uint32 I2 = IndexBuffer[BaseIndex + 2];
//
//			if (Distances.IsValidIndex(I0) && Distances.IsValidIndex(I1) && Distances.IsValidIndex(I2))
//			{
//				const float D0 = Distances[I0];
//				const float D1 = Distances[I1];
//				const float D2 = Distances[I2];
//
//				const float DMin = FMath::Min3(D0, D1, D2);
//				const float DMax = FMath::Max3(D0, D1, D2);
//
//				if (DMin < 0.0f && DMax > 0.0f)
//				{
//					FScopeLock Lock(&IntersectingSetLock);
//					OutIntersectingVertexIndices.Add(I0);
//					OutIntersectingVertexIndices.Add(I1);
//					OutIntersectingVertexIndices.Add(I2);
//				}
//			}
//		});
//
//	for (uint32 i = 0; i < NumVertices; ++i)
//	{
//		if (Distances[i] > 0.0f)
//		{
//			OutPositiveVertexIndices.Add(i);
//		}
//		else
//		{
//			OutNegativeVertexIndices.Add(i);
//		}
//	}
//}
//
