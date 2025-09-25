// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/SBEquipment.h"

#include "GameFramework/Character.h"

// Sets default values
ASBEquipment::ASBEquipment()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

// Called when the game starts or when spawned
void ASBEquipment::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASBEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBEquipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshAsset)
	{
		Mesh->SetSkeletalMesh(MeshAsset);
	}
}

void ASBEquipment::EquipItem()
{
}

void ASBEquipment::UnequipItem()
{
}

void ASBEquipment::AttachToOwner(FName SocketName)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
		{
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
}
