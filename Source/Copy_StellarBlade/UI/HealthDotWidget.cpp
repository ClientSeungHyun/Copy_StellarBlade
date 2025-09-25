// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthDotWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Materials/MaterialInterface.h"

UHealthDotWidget::UHealthDotWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UHealthDotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

    if (Image_HP && Image_HP->Brush.GetResourceObject())
    {
        UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Blueprints/Material/M_UI_HPDot"));

        if (Material)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            Image_HP->SetBrushFromMaterial(DynamicMaterial);

            DynamicMaterial->SetScalarParameterValue(FName("Ratio"), 0.5f);
        }
    }
}

void UHealthDotWidget::SetRatio(float Ratio) const
{
}
