// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Blueprint/UserWidget.h"
#include "EvePotionUI.generated.h"

class UImage;

UCLASS()
class COPY_STELLARBLADE_API UEvePotionUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//////////////////////////////////////
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PotionVerticalBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PotionCountText;

	UPROPERTY(EditAnywhere, Category = "Image")
	UTexture2D* PotionFillTex;

	UPROPERTY()
	TArray<UImage*> PotionImages;

public:
	UEvePotionUI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct();


protected:
	void OnAttributeChanged(EAttributeType AttributeType, float InValue);

private:
	void UpdatePotionUI(float InValue);
};
