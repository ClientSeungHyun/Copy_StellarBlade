// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define COLLISION_OBJECT_INTERACTION ECC_GameTraceChannel1

UENUM(BlueprintType)
enum class ESBAttributeType : uint8
{
    Stamina,
    Health,
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front,
    Back,
    Left,
    Right,
};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
    None,
    SwordShield,
    TwoHanded,
    MeleeFists,
    OneTentacle,
    TwoTentacle
};

UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
    MainCollision,
    SecondCollision,
};

UENUM(BlueprintType)
enum class ESBAIBehavior : uint8
{
    Idle,
    Patrol,
    MeleeAttack,
    Approach,
};