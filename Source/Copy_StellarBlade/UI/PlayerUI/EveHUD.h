// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Blueprint/UserWidget.h"
#include "EveHUD.generated.h"

class UEveHpBar;

UCLASS()
class COPY_STELLARBLADE_API UEveHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//////////////////////////////////////
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* HPGrid;

	UPROPERTY(EditAnywhere, Category = "HP")
	int32 NumColumns = 20;

	UPROPERTY(EditAnywhere, Category = "HP")
	int32 NumRows = 3;

	UPROPERTY(EditAnywhere, Category = "HP")
	UTexture2D* HPEmptyTexture;	
	
	UPROPERTY(EditAnywhere, Category = "HP")
	UTexture2D* HPFillTexture;

	void UpdateHP(float Ratio);

public:
	UEveHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct();

protected:
	void OnAttributeChanged(EAttributeType AttributeType, float InValue);
};
