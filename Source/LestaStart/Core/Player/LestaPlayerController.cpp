// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "LestaCharacter.h"
#include "Kismet/GameplayStatics.h"

ALestaSpectator* ALestaPlayerController::SpawnSpectatorPawn()
{
	ALestaSpectator* SpawnedSpectator = nullptr;
	bool bDoesSpectatorPawnExist = GetSpectatorPawn() == nullptr;
	bool bIsLocal = IsLocalController();
	if (bDoesSpectatorPawnExist && bIsLocal && GetWorld())
	{
		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transient;
		Params.Owner = this;
		FTransform SpawnedTransform = FTransform(GetControlRotation(), GetSpawnLocation());
		auto* SpawnedSpectator1 = GetWorld()->SpawnActor<ALestaSpectator>(SpectatorToSpawn->GetDefaultObject<ALestaSpectator>()->GetClass(),  GetSpawnLocation(), GetControlRotation(), Params);
		//SpawnedSpectator = GetWorld()->SpawnActorDeferred<ALestaSpectator>(SpectatorToSpawn->StaticClass(),
		//	SpawnedTransform, this);
		
	
		if (SpawnedSpectator)
		{
			SpawnedSpectator->SetReplicates(false);
			SpawnedSpectator->PossessedBy(this);
			SpawnedSpectator->EnableInput(this);
			//SpawnedSpectator->RequestRestartAction = SpectatorToSpawn->GetDefaultObject<ALestaSpectator>()->RequestRestartAction;
			//UGameplayStatics::FinishSpawningActor(SpawnedSpectator, SpawnedTransform);
			SetupInputComponent();
			SpawnedSpectator->SetupPlayerInputComponent(InputComponent);

			

			ChangeState(NAME_Spectating);
			ClientGotoState(NAME_Spectating);

		} 
	}
	
	return SpawnedSpectator;
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
