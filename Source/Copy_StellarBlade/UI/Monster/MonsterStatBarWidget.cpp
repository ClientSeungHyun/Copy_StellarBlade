// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterStatBarWidget.h"
#include "Components/SBAttributeComponent.h"
#include "UI/Monster/MonsterHealthBarWidget.h"

UMonsterStatBarWidget::UMonsterStatBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super{ ObjectInitializer }
{
}

void UMonsterStatBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (USBAttributeComponent* Attribute = OwningPawn->GetComponentByClass<USBAttributeComponent>())
		{
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastAttributeChanged(ESBAttributeType::Health);
			Attribute->BroadcastAttributeChanged(ESBAttributeType::Stamina);
		}
	}
}

void UMonsterStatBarWidget::OnAttributeChanged(ESBAttributeType AttributeType, float InRatio)
{
	switch (AttributeType)
	{
	case ESBAttributeType::Health:
		HealthBarWidget->SetRatio(InRatio);
		break;
	//case ESBAttributeType::Stamina:
	//	StaminaBarWidget->SetRatio(InValue);
	//	break;

	}
}
