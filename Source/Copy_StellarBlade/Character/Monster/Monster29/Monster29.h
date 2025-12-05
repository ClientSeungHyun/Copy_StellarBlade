// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Monster29.generated.h"

UENUM(BlueprintType)
enum class EMonster29Type : uint8
{
	Attacker,
	Deffender,
};

UCLASS()
class COPY_STELLARBLADE_API AMonster29 : public AMonsterCharacter
{
	GENERATED_BODY()
	
public:
	AMonster29();

public:
	UPROPERTY(EditAnywhere)
	EMonster29Type MonsterType = EMonster29Type::Attacker;

	// Upper, Lower Body Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster29|Body")
	USkeletalMesh* UpperBodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster29|Body")
	USkeletalMesh* LowerBodyMesh;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;

protected:
	virtual void OnDeath();

};
