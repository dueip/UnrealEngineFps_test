﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FCompletelyDrainedDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FStartedReloadingDelegate, /* ReloadTime */ float,   /* MaxCapacity */ float);


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
	virtual bool IsAtFullCapacity() { return false; }

	virtual bool IsDrained() { return true; }
	virtual void Reload() {};
	virtual float GetReloadTime() { return 0.f; }
	virtual int32 GetMaxDrainage() { return 0;}
	virtual float GetCurrentDrainage() { return 0.f;}

	UFUNCTION(Server, Unreliable)
	virtual void ServerShoot();
	UFUNCTION(Server, Unreliable)
	virtual void ServerShootAt(const FVector& Origin);
	UFUNCTION(Server, Unreliable)
	virtual void ServerReload();
	UFUNCTION(Server, Unreliable)
	virtual void ServerStopShooting();
	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulticastDrawShooting();

	virtual void DrawShooting() {};
	
	virtual FCompletelyDrainedDelegate* GetCompletelyDrainedDelegate() { return nullptr; }
	virtual FStartedReloadingDelegate* GetStartedReloadingDelegate() { return nullptr; }
	
	// Мне не очень нравится это делать, но через сабкомпонент получится довольно плохо.
	virtual FName GetDisplayName() const { return ""; } 
};
