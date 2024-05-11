// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeTurret.h"

#include "LestaStart/Core/Weapons/WeaponInterface.h"


// Sets default values
AGrenadeTurret::AGrenadeTurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGrenadeTurret::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponComponent)
	{
		WeaponComponent->ServerStopShooting();
	}
	
}

void AGrenadeTurret::OnShoot()
{
	
	if (WeaponComponent)
	{
		WeaponComponent->ServerShoot();
	}
}

void AGrenadeTurret::OnStopShooting()
{
	if (WeaponComponent)
	{
		WeaponComponent->ServerStopShooting();
	}
	TimerBetweenShotsHandle.Invalidate();
	
}


// Called every frame
void AGrenadeTurret::Tick(float DeltaTime)
{
	
	
	if (CurrentMode == Modes::Attacking || bHasBeenTriggered)
	{
		bHasBeenTriggered = true;
		OnShoot();
		if (WeaponComponent)
		{
			WeaponComponent->MulticastDrawShooting();
			if (WeaponComponent->IsAtFullCapacity())
			{
				WeaponComponent->ServerStopShooting();
				ServerRequestDestroy();
			}
		}
	}
	if (!bHasBeenTriggered)
	{
		Super::Tick(DeltaTime);
	}
	
}

