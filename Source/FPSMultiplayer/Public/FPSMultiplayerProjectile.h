// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSMultiplayerProjectile.generated.h"

class USphereComponent;
class UBoxComponent;
class UProjectileMovementComponent;
class UMaterialInstanceDynamic;
class UMaterialInstance;
class UStaticMeshComponent;
class UParticleSystem;
class UDamageType;
class URadialForceComponent;

UCLASS(config=Game)
class AFPSMultiplayerProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UBoxComponent* OverlayBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Explode")
		URadialForceComponent* RadialForceComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	FTimerHandle ExplosionAnimationDelay_TimeHandler;
	FTimerHandle ExplosionDelay_TimeHandler;

	FName ExplosionSpeedParameterName;

	UPROPERTY(ReplicatedUsing = OnRep_ExplosionSpeedParameterValue)
		float ExplosionSpeedParameterValue;

	UPROPERTY(ReplicatedUsing = OnRep_bIsExploaded)
		bool bIsExploaded;

	UMaterialInstanceDynamic* ExplosionDynamicMaterial;

	void Blink();
	void Expload();
	void PlayExplosionEffect();

	UFUNCTION()
	void OnRep_ExplosionSpeedParameterValue();

	UFUNCTION()
	void OnRep_bIsExploaded();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	AFPSMultiplayerProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPickUp();

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

