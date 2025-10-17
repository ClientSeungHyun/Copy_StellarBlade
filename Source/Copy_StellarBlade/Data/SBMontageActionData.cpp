// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/SBMontageActionData.h"

UAnimMontage* USBMontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index) const
{
	if (MontageGroupMap.Contains(GroupTag))
	{
		const FSBMontageGroup& MontageGroup = MontageGroupMap[GroupTag];

		if (MontageGroup.Animations.Num() > 0 && MontageGroup.Animations.Num() > Index)
		{
			return MontageGroup.Animations[Index];
		}
	}

	return nullptr;
}

UAnimMontage* USBMontageActionData::GetRandomMontageForTag(const FGameplayTag& GroupTag) const
{
	if (MontageGroupMap.IsEmpty())
		return nullptr;

	if (MontageGroupMap.Contains(GroupTag))
	{
		const FSBMontageGroup& MontageGroup = MontageGroupMap[GroupTag];

		// ·£´ý Index
		const int32 RandomIndex = FMath::RandRange(0, MontageGroup.Animations.Num() - 1);

		return GetMontageForTag(GroupTag, RandomIndex);
	}

	return nullptr;
}

bool USBMontageActionData::IsHaveMontageForTag(const FGameplayTag& GroupTag) const
{
	if (MontageGroupMap.IsEmpty())
		return false;

	return true;
}
