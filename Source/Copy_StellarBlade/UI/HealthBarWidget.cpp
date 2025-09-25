// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthBarWidget.h"

#include "Components/HorizontalBox.h"
#include "UI/HealthDotWidget.h"

void UHealthBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UHealthBarWidget::InitHealthBar(int32 MaxHealth)
{
	if (!HP_Top || !HP_Bottom) return;

	// 초기화
	HP_Top->ClearChildren();
	HP_Bottom->ClearChildren();
	Array_HP_Top.Empty();
	Array_HP_Bottom.Empty();

	// 상/하 똑같이 추가
	for (int32 i = 0; i < (int32)MaxHealth; ++i)
	{
		// Top
		if (UHealthDotWidget* DotTop = CreateWidget<UHealthDotWidget>(GetWorld(), UHealthDotWidget::StaticClass()))
		{
			HP_Top->AddChildToHorizontalBox(DotTop);
			Array_HP_Top.Add(DotTop);
		}

		// Bottom
		if (UHealthDotWidget* DotBottom = CreateWidget<UHealthDotWidget>(GetWorld(), UHealthDotWidget::StaticClass()))
		{
			HP_Bottom->AddChildToHorizontalBox(DotBottom);
			Array_HP_Bottom.Add(DotBottom);
		}
	}

	UpdateHealthUI();
}

void UHealthBarWidget::SetRatio(float Ratio)
{
	Ratio = FMath::Clamp(Ratio, 0.f, 1.f);

	// 배열 크기 기준으로 칸 개수 계산
	const int32 NumDots = FMath::Min(Array_HP_Top.Num(), Array_HP_Bottom.Num());
	int32 ActiveDots = FMath::RoundToInt(Ratio * NumDots);

	for (int32 i = 0; i < NumDots; i++)
	{
		bool bActive = (i < ActiveDots);

		/*if (Array_HP_Top.IsValidIndex(i) && Array_HP_Top[i])
		{
			Array_HP_Top[i]->SetActive(bActive);
		}

		if (Array_HP_Bottom.IsValidIndex(i) && Array_HP_Bottom[i])
		{
			Array_HP_Bottom[i]->SetActive(bActive);
		}*/
	}
}

void UHealthBarWidget::UpdateHealthUI()
{
	
}
