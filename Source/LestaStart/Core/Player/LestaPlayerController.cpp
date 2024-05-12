// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaPlayerController.h"

#include "BlueprintEditor.h"
#include "EnhancedInputSubsystems.h"
#include "LestaCharacter.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "LestaStart/Core/LestaGameMode.h"
#include "Net/UnrealNetwork.h"

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
		if (PlayerState)
		{
			PlayerState->SetIsSpectator(true);
			bWantsToSpectate = true;
			ChangeState(NAME_Spectating);
			ClientGotoState(NAME_Spectating);
		}
			
	}
	return SpawnedSpectator;
}

void ALestaPlayerController::ChangeState(FName NewState)
{
	Super::ChangeState(NewState);
	
}

void ALestaPlayerController::ClientGotoState_Implementation(FName NewState)
{
	Super::ClientGotoState_Implementation(NewState);
	
}

void ALestaPlayerController::ServerViewNextPlayer_Implementation()
{
	// TODO: For whatever reason whenever we try to actually fire this on a client our State is set to Inactive
	// From whatever place that I cannot debug, so I'll leave this code here. At least for now.
	ChangeState(NAME_Spectating);
	ClientGotoState(NAME_Spectating);
	//UE_LOG(LogTemp, Warning, TEXT("Is in state %i"), IsInState(NAME_Spectating));
	Super::ServerViewNextPlayer_Implementation();
}

void ALestaPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bWantsToSpectate)
}


// TODO: Move to gamemode

void ALestaPlayerController::ServerVoteForRestart_Implementation()
{
	if (!GetWorld()) return;
	
	ALestaGameState* LestaGameState = dynamic_cast<ALestaGameState*>(GetWorld()->GetGameState());
	ALestaGameMode* LestaGameMode = dynamic_cast<ALestaGameMode*>(GetWorld()->GetAuthGameMode());

	// If we're travelling don't do anything!
	if (GetWorldTimerManager().IsTimerActive(LestaGameMode->GetTravelTimerHandler()))
	{
		return;
	}
	
	if (LestaGameState && LestaGameMode)
	{
		LestaGameState->Vote(EVoteType::RestartGame);
		
		if (LestaGameState->GetVotedOnRestart() == LestaGameMode->GetHowManyPlayersNeedToVoteOnRestart())
		{
			if (LestaGameState->VoteEndedDelegate.IsBound())
			{
				LestaGameState->VoteEndedDelegate.Broadcast(
					EVoteType::RestartGame,
					LestaGameState->GetVotedOnRestart(),
					LestaGameMode->GetHowManyPlayersNeedToVoteOnRestart()
					);
			}
			
			auto* World = GetWorld();
			
			GetWorldTimerManager().SetTimer(
				LestaGameMode->GetTravelTimerHandler(),
				[World]()
				{
					World->ServerTravel(World->GetMapName());
				},
				LestaGameMode->GetTimeForRestart(),
				false
				);
			
			
		}
	}
}

void ALestaPlayerController::BeginPlay()
{
	bWantsToSpectate = false;
	Super::BeginPlay();
	if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(InputMapping, InputMappingPriority);
		}
	}
}
