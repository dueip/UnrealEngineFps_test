// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"


UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};


class IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void Shoot() PURE_VIRTUAL();
	virtual void StopShooting() PURE_VIRTUAL();
	virtual bool IsCurrentlyShooting() { return false; };
};
