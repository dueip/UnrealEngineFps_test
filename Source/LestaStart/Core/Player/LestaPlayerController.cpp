﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "LestaCharacter.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"

ALestaSpectator* ALestaPlayerController::SpawnSpectatorPawnDifferent()
{
	ALestaSpectator* SpawnedSpectator = nullptr;
	bool bDoesSpectatorPawnExist = GetSpectatorPawn() == nullptr;
	bool bIsLocal = IsLocalController();
	if (bDoesSpectatorPawnExist && bIsLocal && GetWorld())
	{
		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transient;
		Params.Owner = this;
		//SpawnedSpectator = GetWorld()->SpawnActor<ALestaSpectator>(SpectatorToSpawn->GetDefaultObject<ALestaSpectator>()->GetClass(),
		//
		//	GetSpawnLocation(), GetControlRotation(), Params);
		
	
		if (SpawnedSpectator)
		{
			SpawnedSpectator->SetReplicates(false);
			SpawnedSpectator->PossessedBy(this);
			SpawnedSpectator->EnableInput(this);
			SetupInputComponent();	
			SpawnedSpectator->SetupPlayerInputComponent(InputComponent);

			

			ChangeState(NAME_Spectating);
			ClientGotoState(NAME_Spectating);

		} 
	}
	
	return SpawnedSpectator;
}

ASpectatorPawn* ALestaPlayerController::SpawnSpectatorPawn()
{
	ALestaSpectator* SpawnedSpectator = dynamic_cast<ALestaSpectator*>(Super::SpawnSpectatorPawn());
	if (SpawnedSpectator)
	{
		SpawnedSpectator->PossessedBy(this);
		SpawnedSpectator->EnableInput(this);
		SpawnedSpectator->SetupPlayerInputComponent(InputComponent);
		SetupInputComponent();
	}
	return SpawnedSpectator;
}

void ALestaPlayerController::ServerVoteForRestart_Implementation()
{
	if (!GetWorld()) return;
	ALestaGameState* LestaGameState = dynamic_cast<ALestaGameState*>(GetWorld()->GetGameState());
	if (LestaGameState)
	{
		LestaGameState->Vote(EVoteType::RestartGame);
		if (LestaGameState->GetVotedOnRestart() == LestaGameState->GetHowManyPlayersNeedToVoteOnRestart())
		{
			
			GetWorld()->ServerTravel(GetWorld()->GetMapName());
		}
	}
}

void ALestaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(InputMapping, InputMappingPriority);
		}
	}
}
