// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SBDefine.h"
#include "MonsterStatBarWidget.generated.h"

class UMonsterHealthBarWidget;

UCLASS()
class COPY_STELLARBLADE_API UMonsterStatBarWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UMonsterHealthBarWidget* HealthBarWidget;

public:
	UMonsterStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

protected:
	void OnAttributeChanged(ESBAttributeType AttributeType, float InRatio);

};
