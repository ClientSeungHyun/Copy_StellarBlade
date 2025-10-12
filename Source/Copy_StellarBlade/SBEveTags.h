// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace SBEveTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Walking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Running);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_JumpStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Falling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_State_Death);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_Attack_NormalAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Eve_Attack_SkillAttack);
}

