// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SBInteract.h"
#include "SBPickupItem.generated.h"

class ASBEquipment;
class USkeletalMeshComponent;

UCLASS()
class COPY_STELLARBLADE_API ASBPickupItem : public AActor, public ISBInteract
{
	GENERATED_BODY()

public:
	ASBPickupItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Interact(AActor* InteractionActor) override;

public:
	FORCEINLINE void SetEquipmentClass(const TSubclassOf<ASBEquipment>& NewEquipmentClass) { EquipmentClass = NewEquipmentClass; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ASBEquipment> EquipmentClass;

};
