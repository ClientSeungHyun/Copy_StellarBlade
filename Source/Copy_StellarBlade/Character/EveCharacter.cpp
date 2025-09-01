// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EveCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/SpringArmComponent.h"

AEveCharacter::AEveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
}

void AEveCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

