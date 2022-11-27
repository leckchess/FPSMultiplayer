// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, class UFPSHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(FPS), meta=(BlueprintSpawnableComponent) )
class FPSMULTIPLAYER_API UFPSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
		float Health;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth;


	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	

	FOnHealthChangedSignature OnHealthChanged;

	float GetMaxHealth() { return MaxHealth; }

	void ttfunc();
};
