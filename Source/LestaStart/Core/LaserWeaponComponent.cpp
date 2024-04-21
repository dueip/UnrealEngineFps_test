// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWeaponComponent.h"
#include "Engine/DamageEvents.h"
#include "WeaponHoldableInterface.h"


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
	Laser->Deactivate();
	BaseColor = Laser->GetColor();

	IWeaponHoldableInterface* Outer = dynamic_cast<IWeaponHoldableInterface*>(GetOuter());
	if (Outer && Outer->CanHoldWeapon())
	{
		Outer->SetWeapon(this);
	}
	
	// Safety checks:
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
	
	if (!Laser->IsActive())
	{
		return;
	}

	// It's fine to use GetSocketLocation in here since it will return the component's transform anyways
	// See: https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/USceneComponent/GetSocketLocation/

	const FVector SocketOrigin = GetSocketLocation(GetAttachSocketName());
	const FRotator SocketRotation = GetSocketRotation(GetAttachSocketName());
	const FVector EndPoint = SocketOrigin + FVector(
		LaserLength * cos(FMath::DegreesToRadians(SocketRotation.Yaw)),
		LaserLength * sin(FMath::DegreesToRadians(SocketRotation.Yaw)),
		LaserLength * sin(FMath::DegreesToRadians(SocketRotation.Pitch)));
	Laser->SetOrigin(SocketOrigin);
	Laser->SetEndPoint(EndPoint);

	
	
	if (IsValid(GetWorld()) && !BlinkAnimationTimer.IsValid())
	{
		FHitResult Hit;
		bool bWasThereAHit = GetWorld()->LineTraceSingleByChannel(Hit, SocketOrigin, EndPoint, ECC_Pawn);
		FPointDamageEvent PointDamage;
		PointDamage.Damage = DamageAmount;
		PointDamage.HitInfo = Hit;
		// Мы вполне можем получить нуллптр тут и нам не надо волноваться об этом:
		// Это просто значит то, что у нас не было оружие присоединено к Актору
		AActor* ActorThatDealtDamage = dynamic_cast<AActor*>(GetOuter());
		if (bWasThereAHit && Hit.bBlockingHit)
		{
			Hit.GetActor()->TakeDamage(DamageAmount, PointDamage, nullptr, ActorThatDealtDamage);
		}
		CalculateAnimationDurationAndSetTimer();
	}
}

void ULaserWeaponComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	Laser->DestroyComponent(bDestroyingHierarchy);
}

void ULaserWeaponComponent::StopShooting()
{
	Super::StopShooting();
	Laser->Deactivate();
}


void ULaserWeaponComponent::Shoot()
{
	Super::Shoot();
	Laser->Activate();
	//Laser->SetComponentTickEnabled(true);
	//ShootDelegate.ExecuteIfBound();
}

void ULaserWeaponComponent::BlinckingAnimationCallback()
{
	Laser->SetColor(Laser->GetColor() == BaseColor ? BlinkColor : BaseColor);
	BlinkAnimationTimer.Invalidate();
}

