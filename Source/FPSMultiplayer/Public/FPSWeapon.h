// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShakeBase;

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
	
public:	
	// Sets default values for this actor's properties
	AFPSWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float BaseDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		UParticleSystem* MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		UParticleSystem* DefaultImpactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		UParticleSystem* FleshImpactEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		UParticleSystem* SmokeEffect;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		FName SmokeEffectTargetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		TSubclassOf<UCameraShakeBase> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AActor> ProjectileClass;

	/*UPROPERTY(ReplicatedUsing = OnRip_HitScanTrace)
		FHitScanTrace HitScanTrace;*/

	/*UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();*/
	
	void PlayFireEffects(FVector SmokeTrailEndPoint);
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

public:
	void Fire();

};
