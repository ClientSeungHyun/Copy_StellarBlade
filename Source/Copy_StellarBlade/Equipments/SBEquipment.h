// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBEquipment.generated.h"

UCLASS()
class COPY_STELLARBLADE_API ASBEquipment : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Equipment | Mesh")
	UStaticMesh* MeshAsset;

	UPROPERTY(VisibleAnywhere, Category = "Equipment | Mesh")
	UStaticMeshComponent* Mesh;

public:
	ASBEquipment();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void EquipItem();

	virtual void UnequipItem();

	virtual void AttachToOwner(FName SocketName);

};
