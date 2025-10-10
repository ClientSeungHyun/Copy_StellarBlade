// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Animnotify_ChangeAnimRate.h"

UAnimnotify_ChangeAnimRate::UAnimnotify_ChangeAnimRate(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAnimnotify_ChangeAnimRate::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance) return;

	// 현재 실행 중인 몽타주들 중에서 재생 중인 인스턴스 찾기
	const TArray<FAnimMontageInstance*>& MontageInstances = AnimInstance->MontageInstances;
	for (FAnimMontageInstance* MontageInstance : MontageInstances)
	{
		if (MontageInstance && MontageInstance->IsActive())
		{
			MontageInstance->SetPlayRate(NewPlayRate);

			// 디버그 로그
			if (AActor* Owner = MeshComp->GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: Montage speed set to %.2f"), *Owner->GetName(), NewPlayRate);
			}
		}
	}
}
