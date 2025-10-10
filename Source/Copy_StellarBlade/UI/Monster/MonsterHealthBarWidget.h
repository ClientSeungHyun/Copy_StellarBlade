// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthBarWidget.generated.h"

class UImage;
class UOverlay;

UCLASS()
class COPY_STELLARBLADE_API UMonsterHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UOverlay* OverlayImage;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UImage* Image_BG;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UImage* Image_HP;

protected:
	UMaterialInstanceDynamic* DynamicMaterial;

	float	CurrentRatio = 1.f;

public:
	UMonsterHealthBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;

public:
	FORCEINLINE void SetRatio(float Ratio) { CurrentRatio = Ratio; };
};

