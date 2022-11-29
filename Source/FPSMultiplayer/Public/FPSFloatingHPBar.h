// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSFloatingHPBar.generated.h"

class UProgressBar;

UCLASS()
class FPSMULTIPLAYER_API UFPSFloatingHPBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* HP_Bar;

public:
	void UpdateHPBar(float Health, float MaxHealth);
};
