// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterHealthBarWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Materials/MaterialInterface.h"

UMonsterHealthBarWidget::UMonsterHealthBarWidget(const FObjectInitializer& ObjectInitializer)
    :Super{ ObjectInitializer }
{
}

void UMonsterHealthBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (Image_HP && Image_HP->Brush.GetResourceObject())
    {
        UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Blueprints/Material/M_UI_HPDot"));

        if (Material)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            Image_HP->SetBrushFromMaterial(DynamicMaterial);

            DynamicMaterial->SetScalarParameterValue(FName("Ratio"), 1.f);
        }
    }
}

void UMonsterHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (DynamicMaterial)
    {
        CurrentRatio = FMath::FInterpTo(CurrentRatio, TargetRatio, InDeltaTime, 2.5f);
        DynamicMaterial->SetScalarParameterValue(FName("Ratio"), CurrentRatio);
    }
}

void UMonsterHealthBarWidget::SetRatio(float Ratio)
{
    TargetRatio = Ratio;
}
