// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "FPSMultiplayerCharacter.h"
#include "FPSWeapon.h"
#include "FPSHealthIndicator.h"
#include "Components\TextBlock.h"
#include "Components\ProgressBar.h"


void UFPSHUD::Init()
{
	
	if(HP_Bar)
		HP_Bar->SetFillColorAndOpacity(FLinearColor::Green);

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

	if (HP_Bar)
	{
		HP_Bar->SetPercent((Health / MaxHealth));
		
		if (HP_Bar->Percent < 0.5f)
			HP_Bar->SetFillColorAndOpacity(FLinearColor::Red);
		else if (HP_Bar->Percent < 0.75f)
			HP_Bar->SetFillColorAndOpacity(FLinearColor::Yellow);
		else
			HP_Bar->SetFillColorAndOpacity(FLinearColor::Green);
	}
}

void UFPSHUD::OnAmmoChanged(AFPSWeapon* Weapon, int AmmoCount)
{
	if (AmmoCount_Text)
	{
		AmmoCount_Text->SetText(FText::AsNumber(AmmoCount));

		if (AmmoCount == 0)
			Warning_Text->SetVisibility(ESlateVisibility::Visible);
		else
			Warning_Text->SetVisibility(ESlateVisibility::Collapsed);
	}
}

