// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerUI/EveSkillUI.h"
#include "Components/SBEveAtrributeComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UEveSkillUI::UEveSkillUI(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UEveSkillUI::NativeConstruct()
{
    Super::NativeConstruct();

    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (USBEveAtrributeComponent* Attribute = OwningPawn->GetComponentByClass<USBEveAtrributeComponent>())
        {
            Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
        }
    }

    SkillIconImage->SetOpacity(0.3f);
    SkillEnergyAmoutImage->SetOpacity(0.3f);
    SkillText->SetRenderOpacity(0.3f);
    SkillGlowImage->SetOpacity(0.f);
}

void UEveSkillUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bActiveGlow)
    {
        // 불투명도 변화
        float Change = OpacitySpeed * InDeltaTime;

        if (bIncreasing)
        {
            CurrentOpacity += Change;
            if (CurrentOpacity >= 1.0f)
            {
                CurrentOpacity = 1.0f;
                bIncreasing = false; // 다시 줄어들게
            }
        }
        else
        {
            CurrentOpacity -= Change;
            if (CurrentOpacity <= 0.3f)
            {
                CurrentOpacity = 0.3f;
                bIncreasing = true; // 다시 커지게
            }
        }

        SkillGlowImage->SetOpacity(CurrentOpacity);
    }
}

void UEveSkillUI::OnAttributeChanged(EAttributeType AttributeType, float InValue)
{
    switch (AttributeType)
    {
    case EAttributeType::BetaEnergy:
        SetSkillUI(InValue);
        break;
    }
}

void UEveSkillUI::SetSkillUI(float InValue)
{
    if (InValue < 40.f)
    {
        if(bActiveGlow)
        {
            bActiveGlow = false;
            SkillIconImage->SetOpacity(0.3f);
            SkillEnergyAmoutImage->SetOpacity(0.3f);
            SkillText->SetRenderOpacity(0.3f);
            SkillGlowImage->SetOpacity(0.f);
            bIncreasing = true;
        }
        return;
    }

    bActiveGlow = true;
    SkillIconImage->SetOpacity(1.f);
    SkillEnergyAmoutImage->SetOpacity(1.f);
    SkillText->SetRenderOpacity(1.f);
}
