// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMultiplayerGameMode.h"
#include "FPSMultiplayerHUD.h"
#include "FPSMultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSMultiplayerGameMode::AFPSMultiplayerGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSMultiplayerHUD::StaticClass();
}
