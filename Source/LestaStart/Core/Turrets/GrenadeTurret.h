// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turret.h"
#include "LestaStart/Core/Weapons/WeaponHoldableInterface.h"
#include "GrenadeTurret.generated.h"

UCLASS()
class LESTASTART_API AGrenadeTurret : public ATurret, public IWeaponHoldableInterface 
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrenadeTurret();
	virtual bool CanHoldWeapon() const override { return true; }
	virtual bool SetWeapon(IWeaponInterface* Weapon) override { WeaponComponent = Weapon; return true; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnShoot();
	void OnStopShooting();


	bool bHasBeenTriggered = false;

	IWeaponInterface* WeaponComponent;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
