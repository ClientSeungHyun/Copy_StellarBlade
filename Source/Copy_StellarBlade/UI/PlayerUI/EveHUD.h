// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Blueprint/UserWidget.h"
#include "EveHUD.generated.h"

class UImage;

USTRUCT()
struct FBEOverlaySet
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UImage*> Images;
};

UCLASS()
class COPY_STELLARBLADE_API UEveHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//////////////////////////////////////
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* BE_HorizontalBox;
	
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* HPGrid;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* BE_Overlay; 

	UPROPERTY(EditAnywhere, Category = "HP")
	int32 NumColumns = 20;

	UPROPERTY(EditAnywhere, Category = "HP")
	int32 NumRows = 3;
	
	UPROPERTY(EditAnywhere, Category = "HP")
	UTexture2D* HPFillTexture;

	UPROPERTY(EditAnywhere, Category = "Beta")
	TArray<class UTexture2D*> BE_Textures; // 총 4개 이미지 텍스처

	UPROPERTY()
	TArray<UOverlay*> BE_OverlayList;

	UPROPERTY()
	TArray<FBEOverlaySet> BE_OverlayImages;

public:
	UEveHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct();

protected:
	void OnAttributeChanged(EAttributeType AttributeType, float InValue);

	void UpdateHP(float Ratio);

	void UpdateBeta(float BetaValue);
};
