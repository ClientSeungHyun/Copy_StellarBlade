// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterShieldGageWidget.h"
#include "Character/Monster/MonsterCharacter.h"
#include "Components/MonsterAttributeComponent.h"
#include "UI/Monster/MonsterStatBarWidget.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Materials/MaterialInterface.h"

UMonsterShieldGageWidget::UMonsterShieldGageWidget(const FObjectInitializer& ObjectInitializer)
    :Super{ ObjectInitializer }
{
}

void UMonsterShieldGageWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UMonsterShieldGageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMonsterShieldGageWidget::InitGage()
{
    CurrentGage = TargetGage;

    if (!HorizontalBox_BG || !HorizontalBox_Dot) return;

    HorizontalBox_BG->ClearChildren();
    HorizontalBox_Dot->ClearChildren();
    BGImages.Empty();
    DotImages.Empty();

    for (int i = 0; i < TargetGage; ++i)
    {
        UImage* BG = NewObject<UImage>(this);
        HorizontalBox_BG->AddChild(BG);
        BGImages.Add(BG);

        // 점 이미지 생성
        UImage* Dot = NewObject<UImage>(this);
        HorizontalBox_Dot->AddChild(Dot);
        DotImages.Add(Dot);

        FMargin NewMargin = PaddingVector;
        if ((i + 1) % 4 == 0)
        {
            NewMargin.Right += 8.f;
        }

        if (BG)
        {
            BG->SetBrushFromTexture(BGTexture);
            if (UHorizontalBoxSlot* BGSlot = Cast<UHorizontalBoxSlot>(BG->Slot))
            {
                BG->SetRenderScale(FVector2D(0.3f, 0.3f));
                BGSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
                BGSlot->SetPadding(NewMargin); // 좌우 간격
            }
        }

        if (Dot)
        {
            Dot->SetBrushFromTexture(DotTexture);
            if (UHorizontalBoxSlot* DotSlot = Cast<UHorizontalBoxSlot>(Dot->Slot))
            {
                Dot->SetRenderScale(FVector2D(0.3f, 0.3f));
                DotSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
                DotSlot->SetPadding(NewMargin);// 좌우 간격
            }
        }

    }

}

void UMonsterShieldGageWidget::SetGage(int Gage)
{
    TargetGage = CurrentGage = Gage;

    for (int i = 0; i < DotImages.Num(); ++i)
    {
        if (DotImages[i])
        {
            DotImages[i]->SetVisibility(i < CurrentGage ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }
    }
}

