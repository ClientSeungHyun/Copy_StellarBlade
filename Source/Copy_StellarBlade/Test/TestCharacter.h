// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProceduralMeshComponent.h"
#include "TestCharacter.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class COPY_STELLARBLADE_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

	/*Test*/
protected:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComponent;


	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* OtherMeshComponent;;

protected:
	UPROPERTY(EditAnywhere, Category = "Slice")
	FName TargetBoneName = "upperarm_l";

	UPROPERTY(EditAnywhere, Category = "Slice")
	float CreateProceduralMeshDistance = 20.f;

	UPROPERTY(EditAnywhere, Category = "Slice")
	FName ProceduralMeshAttachSocketName = "ProcedrualMesh_Attach";

	UPROPERTY(EditAnywhere, Category = "Slice")
	FName OtherHalfMeshAttachSocketName = "OtherHalfMesh_Attach";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slice")
	UMaterialInterface* CapMaterial;

	int32 NumVertices = 0;
	TMap<int32, int32> VertexIndexMap;
	TArray<FVector> FilteredVerticesArray;
	TArray<int32> Indices;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FColor> Colors;

	FRotator ProcMeshRotation;
	FRotator OtherHalfRotation;

public:
	// Sets default values for this character's properties
	ATestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
public:
	void SelectVertices(int32 LODIndex);
	void ApplyVertexAlphaToSkeletalMesh();
	void CopySkeletalMeshToProcedural(int32 LODIndex);
	void SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf);

	FVector GetAverageVertexPosition(const TArray<FVector>& Vertices);

	//void CreateProceduralMesh();


public:
	//void SliceMesh();
	//USkeletalMesh* BuildVertexToSkeletalMesh(const TSet<uint32>& Indices, const FName& AssetNameTag) const;
	//void SplitVerticesByPlane(const FPlane& SlicePlane,
	//	TSet<uint32>& OutPositiveVertexIndices,
	//	TSet<uint32>& OutNegativeVertexIndices,
	//	TSet<uint32>& OutIntersectingVertexIndices) const;
};
