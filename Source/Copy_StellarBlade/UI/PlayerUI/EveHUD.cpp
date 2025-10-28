// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerUI/EveHUD.h"
#include "Components/SBEveAtrributeComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"

UEveHUD::UEveHUD(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UEveHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (USBEveAtrributeComponent* Attribute = OwningPawn->GetComponentByClass<USBEveAtrributeComponent>())
		{
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastAttributeChanged(EAttributeType::Health);
		}
	}


	if (!HPGrid || !HPEmptyTexture || !HPFillTexture) return;

	HPGrid->ClearChildren();

	int32 TotalBlocks = NumColumns * NumRows;

	for (int32 row = 0; row < NumRows; ++row)
	{
		for (int32 col = 0; col < NumColumns; ++col)
		{
			UImage* Block = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			Block->SetBrushFromTexture(HPFillTexture);

			Block->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));

			UUniformGridSlot* GridSlot = HPGrid->AddChildToUniformGrid(Block, row, col);
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}

void UEveHUD::OnAttributeChanged(EAttributeType AttributeType, float InValue)
{
	switch (AttributeType)
	{
	case EAttributeType::Health:
		UpdateHP(InValue);
		break;
	}
}

void UEveHUD::UpdateHP(float Ratio)
{
	if (!HPGrid) return;

	int32 ActiveColumns = FMath::CeilToInt(Ratio * NumColumns);
	ActiveColumns = FMath::Clamp(ActiveColumns, 0, NumColumns);

	for (int32 Row = 0; Row < NumRows; ++Row)
	{
		for (int32 Col = 0; Col < NumColumns; ++Col)
		{
			int32 Index = Row * NumColumns + Col;
			UImage* Block = Cast<UImage>(HPGrid->GetChildAt(Index));
			if (Block)
			{
				bool bActive = (Col < ActiveColumns); // ← 가로 방향 기준으로 채움 (세로줄 단위)
				FLinearColor Color = bActive
					? FLinearColor::White
					: FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
				Block->SetColorAndOpacity(Color);
			}
		}
	}
}
