// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRPawnCPlusPlusGameMode.h"
#include "VRPawnCPlusPlusCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVRPawnCPlusPlusGameMode::AVRPawnCPlusPlusGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
