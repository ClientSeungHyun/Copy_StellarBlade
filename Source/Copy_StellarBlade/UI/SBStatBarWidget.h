// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SBStatBarWidget.generated.h"

class UProgressBar;

UCLASS()
class COPY_STELLARBLADE_API USBStatBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* StatBar;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatBar")
	FLinearColor FillColorAndOpacity = FLinearColor::Red;

public:
	USBStatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitHealtBar(float MaxHealth = 100.f);

public:
	virtual void NativePreConstruct() override;

public:
	void SetRatio(float Ratio) const;
};
