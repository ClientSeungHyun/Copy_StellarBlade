// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_PlaySound_Once.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"

UAnimNotify_PlaySound_Once::UAnimNotify_PlaySound_Once()
{
    static ConstructorHelpers::FObjectFinder<USoundBase> FootstepSoundObj(
        TEXT("/Game/Sound/Eve/PC_Foot_Default_run_7.PC_Foot_Default_run_7"));
    if (FootstepSoundObj.Succeeded())
        FootstepSound = FootstepSoundObj.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> JumpSoundObj(
        TEXT("/Game/Sound/Eve/Eve_Jump_Sound.Eve_Jump_Sound"));
    if (JumpSoundObj.Succeeded())
        JumpSound = JumpSoundObj.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> LandSoundObj(
        TEXT("/Game/Sound/Eve/Eve_Land_Sound.Eve_Land_Sound"));
    if (LandSoundObj.Succeeded())
        LandSound = LandSoundObj.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> PotionSoundObj(
        TEXT("/Game/Sound/Eve/Eve_Potion.Eve_Potion"));
    if (PotionSoundObj.Succeeded())
        PotionSound = PotionSoundObj.Object; 
    
    static ConstructorHelpers::FObjectFinder<USoundBase> DashSoundObj(
        TEXT("/Game/Sound/Eve/Eve_Dash_Sound.Eve_Dash_Sound"));
    if (DashSoundObj.Succeeded())
        DashSound = DashSoundObj.Object;
}

void UAnimNotify_PlaySound_Once::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    USoundBase* SoundToPlay = nullptr;

    switch (SoundType)
    {
    case ESoundType::Footstep:
        SoundToPlay = FootstepSound;
        break;

    case ESoundType::Jump:
        SoundToPlay = JumpSound;
        break;

    case ESoundType::Land:
        SoundToPlay = LandSound;
        break;  
    
    case ESoundType::Potion:
        SoundToPlay = PotionSound;
        break; 
    
    case ESoundType::Dash:
        SoundToPlay = DashSound;
        break;
    
    case ESoundType::SwordSwish:
        SoundToPlay = SwordSoundList[SelectedIndex];
        break;
    }

    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), SoundToPlay, MeshComp->GetComponentLocation());
    }
}
