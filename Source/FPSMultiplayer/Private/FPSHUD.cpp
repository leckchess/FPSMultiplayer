// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "FPSMultiplayerCharacter.h"
#include "FPSWeapon.h"
#include "FPSHealthIndicator.h"
#include "Components\TextBlock.h"


void UFPSHUD::Init()
{
	if (AFPSMultiplayerCharacter* PlayerCharacter = Cast<AFPSMultiplayerCharacter>(GetOwningPlayerPawn()))
	{
		if (!PlayerCharacter->IsDead())
		{
			if (AFPSWeapon* PlayerCurrentWeapon = PlayerCharacter->GetCurrentWeapon())
			{
				PlayerCurrentWeapon->OnAmmoChanged.AddDynamic(this, &UFPSHUD::OnAmmoChanged);

				if (AmmoCount_Text)
				{
					AmmoCount_Text->SetText(FText::AsNumber(PlayerCurrentWeapon->GetCurrentAmmoInClip()));
				}
			}
		}
	}
}

void UFPSHUD::OnHealthUpdated(float Health, float MaxHealth)
{
	if (HealthIndicator_WBP)
		HealthIndicator_WBP->OnHealthUpdated(Health, MaxHealth);
}

void UFPSHUD::OnAmmoChanged(AFPSWeapon* Weapon, int AmmoCount)
{
	if (AmmoCount_Text)
	{
		AmmoCount_Text->SetText(FText::AsNumber(AmmoCount));
	}
}

