// Fill out your copyright notice in the Description page of Project Settings.



#include "LaserTurret.h"

#include "NavigationSystemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "LestaStart/Core/Weapons/LaserWeaponComponent.h"


class ULaserWeaponComponent;
// Sets default values
ALaserTurret::ALaserTurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

// Called when the game starts or when spawned
void ALaserTurret::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponComponent)
	{
		WeaponComponent->StopShooting();
	}
}

void ALaserTurret::OnShoot()
{
	
	if (WeaponComponent && !WeaponComponent->IsDrained())
	{
		WeaponComponent->Shoot();
	}
	else if (WeaponComponent->IsDrained())
	{
		OnStopShooting();
		if (GetWorldTimerManager().IsTimerActive(ReloadTimerHandle))
		{
			return;
		}
		GetWorldTimerManager().SetTimer(
			ReloadTimerHandle,
			this,
			&ALaserTurret::ReloadWeapon,
			WeaponComponent->GetReloadTime()
		);
	}
}

void ALaserTurret::OnStopShooting()
{
	if (WeaponComponent)
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
		
		
		if (ULaserWeaponComponent* LaserWeapon = FindComponentByClass<ULaserWeaponComponent>())
		{
			LaserWeapon->DesiredEndPoint = LaserWeapon->CalculateDefaultEndPoint();
		}

		OnShoot();
		
		if (PlayerPawn && !CheckIfPawnIsInTheFOV(PlayerPawn))
		{
			OnStopShooting();
			ChangeStateTo(Modes::Scouting);
		}
	}
}

void ALaserTurret::ReloadWeapon() const
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

