// Fill out your copyright notice in the Description page of Project Settings.


#include "SBEveTags.h"

namespace SBEveTags
{
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Walking, "Eve.State.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Running, "Eve.State.Running");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Idle, "Eve.State.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_JumpStart, "Eve.State.JumpStart");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Falling, "Eve.State.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Attacking, "Eve.State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Death, "Eve.State.Death");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Dodge, "Eve.State.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Eve_State_Hit, "Eve.State.Hit");

	UE_DEFINE_GAMEPLAY_TAG(Eve_Attack_NormalAttack, "Eve.Attack.NormalAttack");
	UE_DEFINE_GAMEPLAY_TAG(Eve_Attack_SkillAttack, "Eve.Attack.SkillAttack");
	UE_DEFINE_GAMEPLAY_TAG(Eve_Attack_BlinkAttack, "Eve.Attack.BlinkAttack");
}
