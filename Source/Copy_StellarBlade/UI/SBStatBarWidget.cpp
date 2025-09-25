// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SBStatBarWidget.h"

#include "Components/ProgressBar.h"

USBStatBarWidget::USBStatBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void USBStatBarWidget::InitHealtBar(float MaxHealth)
{
}

void USBStatBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (StatBar)
	{
		StatBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}

void USBStatBarWidget::SetRatio(float Ratio) const
{
	if (StatBar)
	{
		StatBar->SetPercent(Ratio);
	}
}