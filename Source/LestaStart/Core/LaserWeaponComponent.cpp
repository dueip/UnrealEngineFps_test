// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWeaponComponent.h"


// Sets default values for this component's properties
ULaserWeaponComponent::ULaserWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Laser = CreateDefaultSubobject<ULaserComponent>(TEXT("Laser"));
	Laser->SetOrigin(ThisClass::GetComponentTransform().GetLocation());
	Laser->SetColor(FColor::Emerald);
	// ...
}


// Called when the game starts
void ULaserWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ULaserWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Shoot();
	// It's fine to use GetSocketLocation in here since it will return the component's transform anyways
	// See: https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/USceneComponent/GetSocketLocation/
	const FVector SocketOrigin = ThisClass::GetSocketLocation(GetAttachSocketName()); 
	Laser->SetOrigin(SocketOrigin);
	Laser->SetEndPoint(SocketOrigin + FVector(100, 0, 0));
	Laser->SetColor(FColor::MakeRandomColor());
}

void ULaserWeaponComponent::Shoot()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Laser fired!"));	
}

