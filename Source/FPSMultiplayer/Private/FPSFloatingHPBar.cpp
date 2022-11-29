// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSFloatingHPBar.h"
#include "Components\ProgressBar.h"

void UFPSFloatingHPBar::UpdateHPBar(float Health, float MaxHealth)
{
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
