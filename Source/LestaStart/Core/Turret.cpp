// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"


// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BeginningMode = Modes::None;
	ViewRadius = 10.f;
	FOV = 2 * UE_PI;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	// Set the mode to the current
	CurrentMode = BeginningMode;
	if (CurrentMode == Modes::None)
	{
		CurrentMode = Modes::Scouting;
	}
}

void ATurret::ChangeStateTo(const Modes Mode)
{
	CurrentMode = Mode;
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentMode)
	{
	case Modes::Scouting:
		{
			FRotator DeltaRotator = FRotator(0, UE_PI / 2, 0) * DeltaTime * 10.f; 
			AddActorWorldRotation(DeltaRotator);
			CurrentMode = Modes::Activated;
		
			break;
		}
	case Modes::Activated:
		if (!TimerHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::	ChangeStateToAttack, 10.f);
		}
		break;
	case Modes::Attacking:
		{
			FRotator DeltaRotator = FRotator(0, UE_PI / 2, 0) * DeltaTime * 100.f; 
			AddActorWorldRotation(DeltaRotator);
			//CurrentMode = Modes::Activated;
		}
	}
		
}

