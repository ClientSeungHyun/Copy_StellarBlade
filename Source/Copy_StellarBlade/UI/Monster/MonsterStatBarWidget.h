// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SBDefine.h"
#include "MonsterStatBarWidget.generated.h"

class UMonsterHealthBarWidget;
class UMonsterShieldGageWidget;
class AMonsterCharacter;

UCLASS()
class COPY_STELLARBLADE_API UMonsterStatBarWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UMonsterHealthBarWidget* HealthBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UMonsterShieldGageWidget* ShieldGageWidget;

private:
	AMonsterCharacter* OwnerMonster;

public:
	UMonsterStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE AMonsterCharacter* GetOwnerMonster() { return OwnerMonster; };
	FORCEINLINE void SetOwnerMonster(AMonsterCharacter* Owner) { OwnerMonster = Owner; };

protected:
	void OnAttributeChanged(EAttributeType AttributeType, float InRatio);

};
