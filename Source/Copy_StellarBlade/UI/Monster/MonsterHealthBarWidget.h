// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthBarWidget.generated.h"

class UMonsterStatBarWidget;
class UImage;
class UOverlay;

UCLASS()
class COPY_STELLARBLADE_API UMonsterHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	UMonsterStatBarWidget* ParentWidget;

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
	float	TargetRatio = 1.f;

public:
	UMonsterHealthBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetRatio(float Ratio);

public:
	FORCEINLINE void SetParentStatWidget(UMonsterStatBarWidget* InParent) { ParentWidget = InParent; }

};

