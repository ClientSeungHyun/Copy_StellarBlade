// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UHealthDotWidget;
class UHorizontalBox;

UCLASS()
class COPY_STELLARBLADE_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UHorizontalBox* HP_Top;
	UHorizontalBox* HP_Bottom;

	// HP 요소들을 배열로 관리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	TArray<UHealthDotWidget*> Array_HP_Top;

	// HP 요소들을 배열로 관리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	TArray<UHealthDotWidget*> Array_HP_Bottom;

public:
	virtual void NativePreConstruct() override;

	// 초기화
	void InitHealthBar(int32 MaxHealth);

	// HP 감소
	void SetRatio(float Ratio);

	// 내부 업데이트
	void UpdateHealthUI();
};
