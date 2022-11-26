// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "FPSMultiplayerCharacter.h"
#include "FPSWeapon.h"
#include "Components\TextBlock.h"


bool UFPSHUD::Initialize()
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

	return Super::Initialize();
}

void UFPSHUD::OnAmmoChanged(AFPSWeapon* Weapon, int AmmoCount)
{
	if (AmmoCount_Text)
	{
		AmmoCount_Text->SetText(FText::AsNumber(AmmoCount));
	}

	UE_LOG(LogTemp, Error, TEXT("OnAmmoChanged"));
}

