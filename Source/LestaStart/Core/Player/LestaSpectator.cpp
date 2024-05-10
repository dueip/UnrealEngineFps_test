// Fill out your copyright notice in the Description page of Project Settings.


#include "LestaSpectator.h"

#include "EnhancedInputComponent.h"


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

// Called to bind functionality to input
void ALestaSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC)
	{
		EIC->BindAction(RequestRestartAction, ETriggerEvent::Triggered, this, &ThisClass::OnRestartRequested);
	}
}

void ALestaSpectator::ServerVoteOnRestarting_Implementation()
{
	if (!GetWorld()) return;
	ALestaGameState* LestaGameState = dynamic_cast<ALestaGameState*>(GetWorld()->GetGameState());
	if (LestaGameState)
	{
		LestaGameState->ServerVote(EVoteType::RestartGame);
	}
}

void ALestaSpectator::OnRestartRequested()
{
	if (!bHasAlreadyVoted)
	{
		ServerVoteOnRestarting();
		bHasAlreadyVoted = true;
	}
}

