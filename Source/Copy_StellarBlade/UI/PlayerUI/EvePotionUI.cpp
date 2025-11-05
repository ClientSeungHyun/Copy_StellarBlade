// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerUI/EvePotionUI.h"
#include "Components/SBEveAtrributeComponent.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

UEvePotionUI::UEvePotionUI(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UEvePotionUI::NativeConstruct()
{
	Super::NativeConstruct();

    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (USBEveAtrributeComponent* Attribute = OwningPawn->GetComponentByClass<USBEveAtrributeComponent>())
        {
            Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
        }
    }

    if (!PotionVerticalBox || !PotionFillTex)
        return;

    // 먼저 비워주기 (중복 추가 방지)
    PotionVerticalBox->ClearChildren();
    PotionImages.Empty();

    // 포션 이미지 3개 생성
    for (int32 i = 0; i < 3; ++i)
    {
        UImage* PotionImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());

        PotionImage->SetBrushFromTexture(PotionFillTex);

        // VerticalBox에 추가하면서 슬롯 반환
        UVerticalBoxSlot* PotionSlot = PotionVerticalBox->AddChildToVerticalBox(PotionImage);

        if (PotionSlot)
        {
            // 위아래 간격 주기 (상단/하단 패딩)
            PotionSlot->SetPadding(FMargin(0.f, 2.f, 0.f,0.f));

            // 중앙 정렬 (선택사항)
           // PotionSlot->SetHorizontalAlignment(HAlign_Center);
        }

        PotionImages.Add(PotionImage);
    }

    PotionCountText->SetText(FText::AsNumber(3));
}

void UEvePotionUI::OnAttributeChanged(EAttributeType AttributeType, float InValue)
{
    switch (AttributeType)
    {
    case EAttributeType::Potion:
        UpdatePotionUI(InValue);
        break;
    }
}

void UEvePotionUI::UpdatePotionUI(float InValue)
{
    PotionCountText->SetText(FText::AsNumber(FMath::FloorToInt(InValue)));

    for (int32 i = 0; i < PotionImages.Num(); ++i)
    {
        if (i < InValue)
        {
            PotionImages[2-i]->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            PotionImages[2-i]->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
