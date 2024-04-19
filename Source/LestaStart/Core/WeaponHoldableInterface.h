﻿#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponHoldableInterface.generated.h"

class UWeaponComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UWeaponHoldableInterface : public UInterface
{
	GENERATED_BODY()
};

class IWeaponHoldableInterface
{
	GENERATED_BODY()
public:
	virtual bool CanHoldWeapon() const { return false;}
	virtual bool SetWeapon(TObjectPtr<UWeaponComponent> Weapon) { return false; }
}; 
