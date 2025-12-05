// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SBWeaponCollisionComponent.h"
#include "Equipments/SBWeapon.h"
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
    if (!OwnerWeapon)
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

void USBWeaponCollisionComponent::SetOwnerWeaopon(ASBWeapon* InOwnerWeapon)
{
    if(InOwnerWeapon)
        OwnerWeapon = InOwnerWeapon;
}

void USBWeaponCollisionComponent::AddIgnoredActor(AActor* Actor)
{
    IgnoredActors.Add(Actor);
}

void USBWeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor)
{
    IgnoredActors.Remove(Actor);
}

void USBWeaponCollisionComponent::SetTraceName(FName InTraceStartName, FName InTraceEndName)
{
    TraceStartName = InTraceStartName;
    TraceEndName = InTraceEndName;
}

void USBWeaponCollisionComponent::SetAttachmentType(EAttachmentType InAttachmentType)
{
    AttachmentType = InAttachmentType;
}

bool USBWeaponCollisionComponent::CanHitActor(AActor* HitActor)
{
    return AlreadyHitActors.Contains(HitActor) == false;
}

void USBWeaponCollisionComponent::AddHitActor(AActor* HitActor)
{
    AlreadyHitActors.Add(HitActor);
}

void USBWeaponCollisionComponent::CollisionTrace()
{
    TArray<FHitResult> OutHits;

    FVector Start = FVector::ZeroVector;
    FVector End = FVector::ZeroVector;;

    if (AttachmentType == EAttachmentType::Socket)
    {
        Start = WeaponMesh->GetSocketLocation(TraceStartName);
        End = WeaponMesh->GetSocketLocation(TraceEndName);
    }
    else if (AttachmentType == EAttachmentType::Bone)
    {
        USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
        Start = SkeletalMesh->GetBoneLocation(TraceStartName, EBoneSpaces::WorldSpace);
        End = SkeletalMesh->GetBoneLocation(TraceEndName, EBoneSpaces::WorldSpace);
    }

    if (Start == FVector::ZeroVector || End == FVector::ZeroVector)
        return;

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

            if (OwnerWeapon && OwnerWeapon->CanHitActor(HitActor))
            {
                OwnerWeapon->AddHitActor(HitActor);

                // Call OnHitActor Broadcast
                if (OnHitActor.IsBound())
                {
                    OnHitActor.Broadcast(Hit);
                }
            }
            else if(CanHitActor(HitActor))
            {
                AddHitActor(HitActor);

                // Call OnHitActor Broadcast
                if (OnHitActor.IsBound())
                {
                    OnHitActor.Broadcast(Hit);
                }
            }
        }
    }
}
