// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthIndicator.h"
//#include "FPSMultiplayerCharacter.h"
#include "FPSHealthComponent.h"
#include "Components\Image.h"

//bool UFPSHealthIndicator::Initialize()
//{
//	/*if (AFPSMultiplayerCharacter* PlayerCharacter = Cast<AFPSMultiplayerCharacter>(GetOwningPlayerPawn()))
//	{
//		if (!PlayerCharacter->IsDead())
//		{
//			if (UFPSHealthComponent* PlayerHealthComp = PlayerCharacter->GetHealthComponent())
//			{
//				PlayerHealthComp->OnHealthChanged.AddDynamic(this, &UFPSHealthIndicator::OnPlayerHealthChanged);
//			}
//		}
//	}*/
//
//	return Super::Initialize();
//}

void UFPSHealthIndicator::OnPlayerHealthChanged(UFPSHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Current Health %f UI"), Health);

	if (HealthImage && HealthComp)
	{
		float DeltaHealth = 1 - (Health / HealthComp->GetMaxHealth());
		DeltaHealth = FMath::Clamp(DeltaHealth, 0.0f, 1.0f);
		HealthImage->GetDynamicMaterial()->SetScalarParameterValue("Alpha", DeltaHealth);
	}
}
