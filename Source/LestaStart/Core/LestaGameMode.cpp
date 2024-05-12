// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaGameMode.h"

void ALestaGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

		JoinedPlayers.Add(NewPlayer);
}

void ALestaGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	// С другими контроллерами должен разобраться супер.
	if (APlayerController* ExitingPlayer = dynamic_cast<APlayerController*>(Exiting))
	{
		JoinedPlayers.Remove(ExitingPlayer);
	}
}
