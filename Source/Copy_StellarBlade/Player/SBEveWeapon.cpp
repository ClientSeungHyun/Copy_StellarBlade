// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBEveWeapon.h"
#include "SBEveTags.h"
#include "Data/SBMontageActionData.h"

// Sets default values
ASBEveWeapon::ASBEveWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
}

void ASBEveWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASBEveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAnimMontage* ASBEveWeapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

