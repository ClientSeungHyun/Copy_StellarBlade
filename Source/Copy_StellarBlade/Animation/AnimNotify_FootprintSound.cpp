// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FootprintSound.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"

UAnimNotify_FootprintSound::UAnimNotify_FootprintSound()
{
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/Sound/Eve/PC_Foot_Default_run_7.PC_Foot_Default_run_7")
    );

    if (SoundObj.Succeeded())
    {
        FootstepSound = SoundObj.Object;
    }
}

void UAnimNotify_FootprintSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    if (!MeshComp || !FootstepSound)
        return;

    AActor* Owner = MeshComp->GetOwner();
    FVector Loc = MeshComp->GetSocketLocation(FootBoneName);

    UGameplayStatics::PlaySoundAtLocation(Owner, FootstepSound, Loc);
}