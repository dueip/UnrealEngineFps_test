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
	
	BlinkDuration = 0.05f;
	AnimationDuration = 0.1f;
	// ...
}


// Called when the game starts
void ULaserWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Laser->PrimaryComponentTick.SetTickFunctionEnable(false);
	BaseColor = Laser->GetColor();

	
	// Safe checks:
	if(AnimationDuration < BlinkDuration)
	{
		UE_LOG(LogTemp, Error, TEXT("Animation duration should not be lesser than the blink duration"));
	}
}


void ULaserWeaponComponent::CalculateAnimationDurationAndSetTimer()
{
	float TimerDuration = 0.f;
	if (Laser->GetColor() != BaseColor)
	{
		TimerDuration = BlinkDuration;
	}
	else
	{
		TimerDuration = AnimationDuration - BlinkDuration;
	}
	GetWorld()->GetTimerManager().SetTimer(BlinkAnimationTimer, this,
	                                       &ThisClass::BlinckingAnimationCallback, TimerDuration, false);
}

// Called every frame
void ULaserWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Shoot();


	// It's fine to use GetSocketLocation in here since it will return the component's transform anyways
	// See: https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/USceneComponent/GetSocketLocation/

	const FVector SocketOrigin = ThisClass::GetSocketLocation(GetAttachSocketName()); 
	Laser->SetOrigin(SocketOrigin);
	Laser->SetEndPoint(SocketOrigin + FVector(100, 0, 0));
	
	if (IsValid(GetWorld()) && !BlinkAnimationTimer.IsValid())
	{
		CalculateAnimationDurationAndSetTimer();
	}
}



void ULaserWeaponComponent::Shoot()
{
	Laser->PrimaryComponentTick.SetTickFunctionEnable(true);
}

void ULaserWeaponComponent::BlinckingAnimationCallback()
{
	Laser->SetColor(Laser->GetColor() == BaseColor ? BlinkColor : BaseColor);
	BlinkAnimationTimer.Invalidate();
}

