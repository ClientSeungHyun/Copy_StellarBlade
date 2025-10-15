// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterStatBarWidget.h"
#include "Components/MonsterAttributeComponent.h"
#include "UI/Monster/MonsterHealthBarWidget.h"
#include "UI/Monster/MonsterShieldGageWidget.h"
#include "Character/Monster/MonsterCharacter.h"

UMonsterStatBarWidget::UMonsterStatBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super{ ObjectInitializer }
{
}

void UMonsterStatBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OwnerMonster)
	{
		if (UMonsterAttributeComponent* Attribute = OwnerMonster->GetComponentByClass<UMonsterAttributeComponent>())
		{
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastAttributeChanged(EAttributeType::Health);
			Attribute->BroadcastAttributeChanged(EAttributeType::Shield);
			Attribute->BroadcastAttributeChanged(EAttributeType::Stamina);

			ShieldGageWidget->InitGage();
		}
	}
}

void UMonsterStatBarWidget::OnAttributeChanged(EAttributeType AttributeType, float InRatio)
{
	switch (AttributeType)
	{
	case EAttributeType::Health:
		HealthBarWidget->SetRatio(InRatio);
		break;
	case EAttributeType::Shield:
		ShieldGageWidget->SetGage((int)InRatio);
		break;
	//case ESBAttributeType::Stamina:
	//	StaminaBarWidget->SetRatio(InValue);
	//	break;

	}
}
