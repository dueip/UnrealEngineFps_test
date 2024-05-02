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
		WeaponComponent->StopShooting();
	}
	
}

void AGrenadeTurret::OnShoot()
{
	
	if (WeaponComponent)
	{
		WeaponComponent->Shoot();
	}
}

void AGrenadeTurret::OnStopShooting()
{
	if (WeaponComponent)
	{
		WeaponComponent->StopShooting();
	}
	TimerBetweenShotsHandle.Invalidate();
	Destroy();
}


// Called every frame
void AGrenadeTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentMode == Modes::Attacking)
	{
		OnShoot();

		
		if (WeaponComponent->IsAtFullCapacity()) WeaponComponent->StopShooting();
		
		
		if (WeaponComponent && !WeaponComponent->IsCurrentlyShooting())
		{
			OnStopShooting();
		} 
		
	}
}

