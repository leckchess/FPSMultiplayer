// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthIndicator.h"
//#include "FPSMultiplayerCharacter.h"

void UFPSHealthIndicator::BeginPlay()
{
	//if (AFPSMultiplayerCharacter* PlayerCharacter = Cast<AFPSMultiplayerCharacter>(GetOwningPlayerPawn()))
	{

	}
	//PlayerHealthComponent->OnHealthChanged.AddDynamic(this, &AFPSMultiplayerCharacter::OnPlayerHealthChanged);
}

void UFPSHealthIndicator::OnPlayerHealthChanged(UFPSHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	
}
