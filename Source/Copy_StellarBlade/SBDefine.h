// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//#define COLLISION_OBJECT_INTERACTION ECC_GameTraceChannel1
#define COLLISION_OBJECT_TARGETING ECC_GameTraceChannel1

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
    Health,
    Shield,
    Stamina,
    BetaEnergy,
};

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
    Front,
    Back,
    Left,
    Right,
};

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
    Front,
    Front_L,
    Front_R,
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
enum class EWeaponType : uint8
{
    MainWeapon,
    SubWeapon,
};

UENUM(BlueprintType)
enum class EWeaponCollisionType : uint8
{
    MainCollision,
    SecondCollision,
};

UENUM(BlueprintType)
enum class EMonsterAIBehavior : uint8
{
    Idle,
    Patrol,
    Follow,
    Attack,
};

UENUM(BlueprintType)
enum class EWeaponMeshType : uint8
{
    Skeletal,
    Static,
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
    Bone,
    Socket,
};

UENUM(BlueprintType)
enum class EMonster : uint8
{
    Bone,
    Socket,
};

UENUM(BlueprintType)
enum class ESwitchingDirection : uint8
{
    None,
    Left,
    Right,
};