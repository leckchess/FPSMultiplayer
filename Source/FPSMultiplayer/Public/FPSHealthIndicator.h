// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSHealthIndicator.generated.h"

class UImage;

UCLASS()
class FPSMULTIPLAYER_API UFPSHealthIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* HealthImage;

	void OnHealthUpdated(float Health, float MaxHealth);
};
