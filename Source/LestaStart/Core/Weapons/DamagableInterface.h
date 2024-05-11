// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamagableInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UDamagableInterface : public UInterface
{
	GENERATED_BODY()
};


class IDamagableInterface
{
	GENERATED_BODY()

public:
	virtual	void ReceiveDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) PURE_VIRTUAL();
	virtual bool CanRecieveDamageFromFriendlies() const { return false; }; 
};