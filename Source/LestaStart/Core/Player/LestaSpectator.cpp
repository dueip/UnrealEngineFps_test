// Fill out your copyright notice in the Description page of Project Settings.


#include "LestaSpectator.h"

#include "EnhancedInputComponent.h"
#include "LestaPlayerController.h"


// Sets default values
ALestaSpectator::ALestaSpectator()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bHasAlreadyVoted = false;
	
}

void ALestaSpectator::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//SetupPlayerInputComponent(CreatePlayerInputComponent());
}

UInputComponent* ALestaSpectator::CreatePlayerInputComponent()
{
	return Super::CreatePlayerInputComponent();
}

// Called when the game starts or when spawned
void ALestaSpectator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALestaSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALestaSpectator::OnPlayerSpectatingChanged()
{
	if (ALestaPlayerController* PC = GetController<ALestaPlayerController>())
	{
		if (GetWorld() && GetWorld()->GetGameState())
		{
			if (true)//GetWorld()->GetGameState()->PlayerArray.Num() > 1)
			{
				PC->ServerViewNextPlayer();
			}
		}
	}
}

// Called to bind functionality to input
void ALestaSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC)
	{
		EIC->BindAction(RequestRestartAction, ETriggerEvent::Triggered, this, &ThisClass::OnRestartRequested);
		EIC->BindAction(RequestChangeSpectatingPlayer, ETriggerEvent::Triggered, this, &ThisClass::OnPlayerSpectatingChanged);
	}
}

void ALestaSpectator::OnReastarting()
{

}

void ALestaSpectator::ServerVoteOnRestarting_Implementation()
{
	OnReastarting();
}

void ALestaSpectator::OnRestartRequested()
{
	// We dont want sㅋerver to be able to vote in anyways.
	if (!IsLocallyControlled()) { return; }
	if (!bHasAlreadyVoted)
	{
		if (ALestaPlayerController* LestaPCPlayerController = dynamic_cast<ALestaPlayerController*>(GetController()))
		{
			LestaPCPlayerController->ServerVoteForRestart();
		}
		bHasAlreadyVoted = true;
	}
}

