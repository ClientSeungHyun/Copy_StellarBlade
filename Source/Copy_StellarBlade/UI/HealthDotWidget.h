// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthDotWidget.generated.h"

class UImage;
class UOverlay;

UCLASS()
class COPY_STELLARBLADE_API UHealthDotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UOverlay* OverlayImage;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UImage* Image_BG;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UImage* Image_HP;

	float	CurrentRatio = 1.f;

	UMaterialInstanceDynamic* DynamicMaterial;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatBar")
	FLinearColor FillColorAndOpacity = FLinearColor::Red;

public:
	UHealthDotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;

public:
	void SetRatio(float Ratio) const;

};
