// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSMultiplayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class AFPSWeapon;
class UFPSHUD;
class AFPSMultiplayerProjectile;
class UFPSFloatingHPBar;

UCLASS(config = Game)
class AFPSMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

		/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UFPSHUD* HUD;
	AFPSMultiplayerProjectile* CurrentOverlappedProjectile;
	UFPSFloatingHPBar* HPBar_Widget;

	FTimerHandle RegainHealth_TimeHandler;

	void RegainHealth();

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UFUNCTION()
		void OnRep_Health();

public:
	AFPSMultiplayerCharacter();

protected:
	virtual void BeginPlay();

	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFPSMultiplayerProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint8 bUsingMotionControllers : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		TSubclassOf<UCameraShakeBase> FireCameraShake;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
		AFPSWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		TSubclassOf<UUserWidget> HPBar_WidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float RegainHealthColldown;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<AFPSWeapon> WeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
		FName WeaponSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
		bool bIsFiring;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		bool bIsDead;

	/** Fires a projectile. */
	void OnFire();

	void Interact();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void StartCrouch();
	void EndCrouch();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);

	void Respawn();
	TouchData	TouchItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

	UFUNCTION()
		void OnRep_CurrentWeapon();

	void UpdateHUD();

public:

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFPSHUD* GetHUD() { return HUD; }

	UFUNCTION(BlueprintCallable)
		void SetHUD(UFPSHUD* InHUD);

	UFUNCTION()
		void OnDeath();

	void SetCurrentOverlappedProjectile(AFPSMultiplayerProjectile* InProjectile) { CurrentOverlappedProjectile = InProjectile; }
	AFPSMultiplayerProjectile* GetCurrentOverlappedProjectile() { return CurrentOverlappedProjectile; }

	bool IsDead() { return bIsDead; }

	AFPSWeapon* GetCurrentWeapon() { return CurrentWeapon; }
};

