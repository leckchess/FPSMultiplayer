// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, AFPSWeapon*, Weapon, int, AmmoCount);

class USkeletalMeshComponent;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class FPSMULTIPLAYER_API AFPSWeapon : public AActor
{
	GENERATED_BODY()

		UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmoInClip)
		int CurrentAmmoInClip;

	UFUNCTION()
	void OnRep_CurrentAmmoInClip();

public:
	// Sets default values for this actor's properties
	AFPSWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		int StarterAmmoNumber;;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float BaseDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AActor> ProjectileClass;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

public:

	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddAmmo();

	FOnAmmoChangedSignature OnAmmoChanged;

	int GetCurrentAmmoInClip() { return CurrentAmmoInClip; }
};
