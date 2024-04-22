// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInvenotryComponent.h"


// Sets default values for this component's properties
UWeaponInvenotryComponent::UWeaponInvenotryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UWeaponInvenotryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

IWeaponInterface* UWeaponInvenotryComponent::GetWeaponAt(const int32 Index)
{
	return Weapons[Index];
}


void UWeaponInvenotryComponent::PushWeapon(IWeaponInterface* Weapon)
{
	Weapons.Add(Weapon);
}

int32 UWeaponInvenotryComponent::GetInventorySize() const
{
	return Weapons.Num();
}


// Called every frame
void UWeaponInvenotryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

