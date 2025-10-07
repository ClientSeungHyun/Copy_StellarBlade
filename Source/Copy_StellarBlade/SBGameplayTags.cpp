// Fill out your copyright notice in the Description page of Project Settings.


#include "SBGameplayTags.h"

namespace SBGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Attacking, "Character.State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Rolling, "Character.State.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_GeneralAction, "Character.State.GeneralAction");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Death, "Character.State.Death");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Hit, "Character.State.Hit");

	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Equip, "Character.Action.Equip");
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Unequip, "Character.Action.Unequip");
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_HitReaction, "Character.Action.HitReaction");



	UE_DEFINE_GAMEPLAY_TAG(Monster_State_Discovered, "Monster.State.Discovered");
	UE_DEFINE_GAMEPLAY_TAG(Monster_State_Hit, "Monster.State.Hit");

	UE_DEFINE_GAMEPLAY_TAG(Monster_Attack_General, "Monster.Attack.General");
	UE_DEFINE_GAMEPLAY_TAG(Monster_Attack_Blink, "Monster.Attack.Blink");
	UE_DEFINE_GAMEPLAY_TAG(Monster_Attack_Repulse, "Monster.Attack.Repulse");
}