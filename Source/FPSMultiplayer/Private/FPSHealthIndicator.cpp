// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthIndicator.h"
#include "Components\Image.h"

void UFPSHealthIndicator::OnHealthUpdated(float Health, float MaxHealth)
{
	UE_LOG(LogTemp, Log, TEXT("Current Health %f UI"), Health);

	if (HealthImage)
	{
		float DeltaHealth = 1 - (Health / MaxHealth);
		DeltaHealth = FMath::Clamp(DeltaHealth, 0.0f, 1.0f);
		HealthImage->GetDynamicMaterial()->SetScalarParameterValue("Alpha", DeltaHealth);
	}
}
