// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Blueprint/UserWidget.h"
#include "EveSkillUI.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class COPY_STELLARBLADE_API UEveSkillUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* SkillIconImage;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillGlowImage;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillEnergyAmoutImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillText;


private:
	bool bActiveGlow = false;
	float CurrentOpacity = 1.0f;
	bool bIncreasing = true;
	float OpacitySpeed = 1.0f;

public:
	UEveSkillUI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnAttributeChanged(EAttributeType AttributeType, float InValue);

private:
	void SetSkillUI(float InValue);
};
