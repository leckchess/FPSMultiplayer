// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSHUD.generated.h"

class UFPSHealthIndicator;
class AFPSWeapon;
class UTextBlock;
class UProgressBar;

UCLASS()
class FPSMULTIPLAYER_API UFPSHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* AmmoCount_Text;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Warning_Text;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UFPSHealthIndicator* HealthIndicator_WBP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* HP_Bar;

	void Init();
	void OnHealthUpdated(float Health, float MaxHealth);

protected:
	UFUNCTION()
		void OnAmmoChanged(AFPSWeapon* Weapon, int AmmoCount);

};
