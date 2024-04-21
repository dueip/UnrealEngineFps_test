// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.h"
#include "../WeaponHoldableInterface.h"
#include "../WeaponInterface.h"
#include "LaserTurret.generated.h"

UCLASS()
class LESTASTART_API ALaserTurret : public ATurret, public IWeaponHoldableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaserTurret();
	virtual bool CanHoldWeapon() const override { return true; }
	virtual bool SetWeapon(IWeaponInterface* Weapon) override { WeaponComponent = Weapon; return true; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnShoot() override;

	virtual void OnStopShooting() override;

public:
	IWeaponInterface* WeaponComponent;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
