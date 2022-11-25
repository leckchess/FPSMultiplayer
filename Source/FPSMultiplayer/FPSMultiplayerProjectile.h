// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSMultiplayerProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UMaterialInstanceDynamic;
class UMaterialInstance;
class UStaticMeshComponent;
class UParticleSystem;
class UDamageType;

UCLASS(config=Game)
class AFPSMultiplayerProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	FTimerHandle ExplosionAnimationDelay_TimeHandler;
	FTimerHandle ExplosionDelay_TimeHandler;

	FName ExplosionSpeedParameterName;
	float ExplosionSpeedParameterValue;

	UMaterialInstanceDynamic* ExplosionDynamicMaterial;

	void Blink();
	void Expload();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	AFPSMultiplayerProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void PickUp();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		UMaterialInstance* ExploadingMaterialInstance;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float AnimationExplosionTime;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float DefaultExplosionTime;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float PlayerExplosionTime;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		UParticleSystem* ExplosionEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		float DamageRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		TSubclassOf<UDamageType> DamageType;
};

