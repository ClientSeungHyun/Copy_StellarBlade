// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/SBPickupItem.h"


#include "SBDefine.h"
#include "Equipments/SBEquipment.h"

ASBPickupItem::ASBPickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupItemMesh"));
	SetRootComponent(Mesh);

	// Collision 설정.
	//Mesh->SetCollisionObjectType(COLLISION_OBJECT_INTERACTION);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASBPickupItem::BeginPlay()
{
	Super::BeginPlay();

}

void ASBPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBPickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 설정된 장비(무기)아이템의 클래스 정보를 이용해서 PickupItem의 외형을 설정.
	if (EquipmentClass)
	{
		if (ASBEquipment* CDO = EquipmentClass->GetDefaultObject<ASBEquipment>())
		{
			//Mesh->SetStaticMesh(CDO->MeshAsset);
			Mesh->SetSimulatePhysics(true);
		}
	}
}

void ASBPickupItem::Interact(AActor* InteractionActor)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InteractionActor;

	// 장비 아이템을 Spawn하고 캐릭터의 Socket에 장착.
	ASBEquipment* SpawnItem = GetWorld()->SpawnActor<ASBEquipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem)
	{
		SpawnItem->EquipItem();
		Destroy();
	}
}

