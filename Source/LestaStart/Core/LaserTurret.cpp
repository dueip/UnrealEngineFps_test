﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTurret.h"

#include "NavigationSystemTypes.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ALaserTurret::ALaserTurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALaserTurret::BeginPlay()
{
	Super::BeginPlay();
	WeaponComponent->StopShooting();
}

void ALaserTurret::OnShoot()
{
	if (IsValid(WeaponComponent))
	{
		WeaponComponent->Shoot();
	}
}

void ALaserTurret::OnStopShooting()
{
	if (IsValid(WeaponComponent))
	{
		WeaponComponent->StopShooting();
	}
	TimerBetweenShotsHandle.Invalidate();
}


// Called every frame
void ALaserTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (CurrentMode == Modes::Attacking)
	{
		if (!TimerBetweenShotsHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(TimerBetweenShotsHandle, this,
				&ThisClass::OnShoot, TimeBetweenShots);
		}
		
		else if (PlayerPawn && !CheckIfPawnIsInTheFOV(PlayerPawn))
		{
			OnStopShooting();
			ChangeStateTo(Modes::Scouting);
		}
	}
}

