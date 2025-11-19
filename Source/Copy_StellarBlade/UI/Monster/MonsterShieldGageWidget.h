// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterShieldGageWidget.generated.h"

class UMonsterStatBarWidget;
class UImage;
class UOverlay;
class UHorizontalBox;

UCLASS()
class COPY_STELLARBLADE_API UMonsterShieldGageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UMonsterStatBarWidget* ParentWidget;

protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox_BG;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox_Dot;

	UPROPERTY(EditAnywhere, Category = "Shield")
	UTexture2D* DotTexture;

	UPROPERTY(EditAnywhere, Category = "Shield")
	UTexture2D* BGTexture;

protected:
	UPROPERTY()
	TArray<UImage*> DotImages;

	UPROPERTY()
	TArray<UImage*> BGImages;

protected:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere)
	FVector4 PaddingVector = FVector4 {-11.f, -18.f, -13.f, -16.f };

	int	CurrentGage = 4;
	int	TargetGage = 4;

public:
	UMonsterShieldGageWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void InitGage();
	void SetGage(int Gage);

public:
	FORCEINLINE void SetParentStatWidget(UMonsterStatBarWidget* InParent) { ParentWidget = InParent; }

	
};
