// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserWeaponComponent.h"
#include "Turret.generated.h"

UCLASS()
class LESTASTART_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category="Weapon")
	TObjectPtr<ULaserWeaponComponent> LaserWeapon;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
