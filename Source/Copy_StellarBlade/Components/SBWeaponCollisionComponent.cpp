// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBWeaponCollisionComponent.h"
#include "Kismet/KismetSystemLibrary.h"

USBWeaponCollisionComponent::USBWeaponCollisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

void USBWeaponCollisionComponent::BeginPlay()
{
    Super::BeginPlay();


}

void USBWeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsCollisionEnabled)
    {
        CollisionTrace();
    }
}

void USBWeaponCollisionComponent::TurnOnCollision()
{
    AlreadyHitActors.Empty();
    bIsCollisionEnabled = true;
}

void USBWeaponCollisionComponent::TurnOffCollision()
{
    bIsCollisionEnabled = false;
}

void USBWeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* MeshComponent)
{
    WeaponMesh = MeshComponent;
}

void USBWeaponCollisionComponent::AddIgnoredActor(AActor* Actor)
{
    IgnoredActors.Add(Actor);
}

void USBWeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor)
{
    IgnoredActors.Remove(Actor);
}

bool USBWeaponCollisionComponent::CanHitActor(AActor* Actor) const
{
    return AlreadyHitActors.Contains(Actor) == false;
}

void USBWeaponCollisionComponent::CollisionTrace()
{
    TArray<FHitResult> OutHits;

    const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
    const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

    bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetOwner(),
        Start,
        End,
        TraceRadius,
        TraceObjectTypes,
        false,
        IgnoredActors,
        DrawDebugType,
        OutHits,
        true);

    if (bHit)
    {
        for (const FHitResult& Hit : OutHits)
        {
            AActor* HitActor = Hit.GetActor();

            if (HitActor == nullptr)
            {
                continue;
            }

            if (CanHitActor(HitActor))
            {
                AlreadyHitActors.Add(HitActor);

                // Call OnHitActor Broadcast
                if (OnHitActor.IsBound())
                {
                    OnHitActor.Broadcast(Hit);
                }
            }
        }
    }
}
