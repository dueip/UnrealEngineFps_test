// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInterface.h"
#include "WeaponInvenotryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API UWeaponInvenotryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponInvenotryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	IWeaponInterface* GetWeaponAt(const int32 Index);
	
	UFUNCTION(BlueprintCallable)
	const IWeaponInterface* GetWeaponAt(const int32 Index) const; 
	
	UFUNCTION(BlueprintCallable)
	void PushWeapon(IWeaponInterface* Weapon);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<IWeaponInterface*> Weapons;
};
