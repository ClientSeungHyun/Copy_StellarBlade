// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBDefine.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SBWeaponCollisionComponent.generated.h"

class ASBWeapon;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitActor, const FHitResult&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COPY_STELLARBLADE_API USBWeaponCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnHitActor OnHitActor;

public:
	ASBWeapon* OwnerWeapon = {};

protected:
	UPROPERTY(EditAnywhere)
	/** 시작 소켓 */
	FName TraceStartName;

	/** 끝 소켓 */
	UPROPERTY(EditAnywhere)
	FName TraceEndName;

	UPROPERTY(EditAnywhere)
	EAttachmentType AttachmentType = EAttachmentType::Socket;

protected:
	/** Sphere 크기(반지름) */
	UPROPERTY(EditAnywhere)
	float TraceRadius = 20.f;

	/** Trace 대상 ObjectType */
	UPROPERTY(EditAnywhere)
	TArray <TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/** 충돌처리 제외 Actors */
	UPROPERTY(EditAnywhere)
	TArray<AActor*> IgnoredActors;

	/** 디버그 드로우 타입 */
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

protected:
	/** 무기의 MeshComponent */
	// StaticMesh, SkeletalMesh 등
	UPROPERTY()
	UPrimitiveComponent* WeaponMesh;

	bool bIsCollisionEnabled = false;

public:
	USBWeaponCollisionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void TurnOnCollision();

	void TurnOffCollision();

	void SetWeaponMesh(UPrimitiveComponent* MeshComponent);

	void SetOwnerWeaopon(ASBWeapon* InOwnerWeapon);

	void AddIgnoredActor(AActor* Actor);

	void RemoveIgnoredActor(AActor* Actor);

	void SetTraceName(FName InTraceStartName, FName InTraceEndName);

	void SetAttachmentType(EAttachmentType InAttachmentType);

	bool CanHitActor(AActor* HitActor);

	void AddHitActor(AActor* HitActor);

protected:
	void CollisionTrace();

		
};
