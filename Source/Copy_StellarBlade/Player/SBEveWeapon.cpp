// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBEveWeapon.h"

// Sets default values
ASBEveWeapon::ASBEveWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
}

// Called when the game starts or when spawned
void ASBEveWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBEveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

