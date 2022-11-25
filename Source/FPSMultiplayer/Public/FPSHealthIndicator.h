// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSHealthIndicator.generated.h"

class UFPSHealthComponent;

UCLASS()
class FPSMULTIPLAYER_API UFPSHealthIndicator : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION()
		void OnPlayerHealthChanged(UFPSHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	virtual void BeginPlay();

};
