// Copyright Epic Games, Inc. All Rights Reserved.

#include "TD_IciclesGeneratorGameMode.h"
#include "TD_IciclesGeneratorCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATD_IciclesGeneratorGameMode::ATD_IciclesGeneratorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
