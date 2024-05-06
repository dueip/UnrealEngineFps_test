// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeWeaponComponent.h"

#include "WeaponHoldableInterface.h"
#include "Chaos/SpatialAccelerationCollection.h"
#include "Engine/DamageEvents.h"
#include "LestaStart/Core/HealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UGrenadeWeaponComponent::UGrenadeWeaponComponent()
{
	/*
	 * Defaults
	*/
	
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaximumCharge = 100.f;
	ChargeGainPerSecond = 10.f;
	MaximumRadius = 400.f;
	MaximumDamage = 100.f;
	MinimumDamage = 1.f;
	bShouldIgnoreOuter = true;
	MaxAmmo = 3;
	ReloadTime = 1.f;

	// Networking
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UGrenadeWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	ServerReload();
	IWeaponHoldableInterface* Outer = dynamic_cast<IWeaponHoldableInterface*>(GetOuter());
	if (Outer && Outer->CanHoldWeapon())
	{
		Outer->SetWeapon(this);
	}
	
}

void UGrenadeWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGrenadeWeaponComponent, MaximumCharge);
	DOREPLIFETIME(UGrenadeWeaponComponent, ChargeGainPerSecond);
	DOREPLIFETIME(UGrenadeWeaponComponent, MaximumRadius);
	DOREPLIFETIME(UGrenadeWeaponComponent, MaximumDamage);
	DOREPLIFETIME(UGrenadeWeaponComponent, MinimumDamage);
	DOREPLIFETIME(UGrenadeWeaponComponent, bShouldIgnoreOuter);
	DOREPLIFETIME(UGrenadeWeaponComponent, MaxAmmo);
	DOREPLIFETIME(UGrenadeWeaponComponent, ReloadTime);
	
	DOREPLIFETIME(UGrenadeWeaponComponent, CurrentCharge);
	DOREPLIFETIME(UGrenadeWeaponComponent, bIsGainingCharge);
	DOREPLIFETIME(UGrenadeWeaponComponent, CurrentAmmoNumber);
}

void UGrenadeWeaponComponent::ServerStopShooting_Implementation()
{
	StopShooting();
}

void UGrenadeWeaponComponent::Shoot()
{
	bIsGainingCharge = true;
}

void UGrenadeWeaponComponent::ServerShoot_Implementation()
{
	Shoot();
}

TArray<FHitResult> UGrenadeWeaponComponent::SweepSphere(const ECollisionChannel TraceChannel) const
{
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(MaximumRadius);
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(
		Hits,
		GetComponentLocation() + FVector(0.f, 0.f, -MaximumRadius),
		GetComponentLocation() + FVector(0.f, 0.f, MaximumRadius),
		GetComponentQuat(),
		TraceChannel,
		CollisionSphere
	);
	return Hits;
}

float UGrenadeWeaponComponent::CalculateCurrentDamageBasedOffCurrentCharge() const
{
	float Damage = MaximumDamage * CalculateCurrentChargeBasedOffMaxCharge();
	return (Damage > MinimumDamage ? Damage : MinimumDamage);
}

bool UGrenadeWeaponComponent::IsBlockedByOtherElement(const FVector& StartPoint, const FVector& EndPoint,
	ECollisionChannel CollisionChannel, const AActor* const HitActor) const
{

	FHitResult TraceHit;
	GetWorld()->LineTraceSingleByChannel(TraceHit, StartPoint, EndPoint, CollisionChannel);

	return TraceHit.bBlockingHit && TraceHit.GetActor() != HitActor;
}

void UGrenadeWeaponComponent::DoDamage_Implementation(const TArray<FHitResult>& Hits) const
{
	for (const FHitResult& Hit : Hits)
	{
		if (Hit.bBlockingHit && (Hit.GetActor() != GetOuter() || !bShouldIgnoreOuter))
		{
			FRadialDamageEvent DamageEvent;
			DamageEvent.Origin = GetComponentLocation();
			
			
			AActor* DamageCauser = dynamic_cast<AActor*>(GetOuter());
			if (IsValid(Hit.GetActor()) && Hit.GetActor()->FindComponentByClass<UHealthComponent>())
			{
				Hit.GetActor()->TakeDamage(CalculateCurrentDamageBasedOffCurrentCharge(),
					DamageEvent, nullptr, DamageCauser);
			} 
		} 
	}
}
	
void UGrenadeWeaponComponent::StopShooting()
{
	if (bIsGainingCharge)
	{
		const TArray<FHitResult> Hits = SweepSphere(ECC_Pawn);
		DoDamage(Hits);
		CurrentAmmoNumber -= 1;
	}
	
 	bIsGainingCharge = false;
	CurrentCharge = 0.f;
}

bool UGrenadeWeaponComponent::IsAtFullCapacity()
{
	return (CurrentCharge >= MaximumCharge);
}

bool UGrenadeWeaponComponent::IsDrained()
{
	return CurrentAmmoNumber <= 0;
}

void UGrenadeWeaponComponent::Reload()
{
	CurrentAmmoNumber = MaxAmmo;
}

float UGrenadeWeaponComponent::GetReloadTime()
{
	return ReloadTime;
}

float UGrenadeWeaponComponent::GetCurrentDrainage()
{
	return static_cast<float>(CurrentAmmoNumber);
}

int32 UGrenadeWeaponComponent::GetMaxDrainage()
{
	return MaxAmmo;
}

FName UGrenadeWeaponComponent::GetDisplayName() const
{
	return "Grenade";
}



void UGrenadeWeaponComponent::ServerReload_Implementation()
{
	Reload();
}

void UGrenadeWeaponComponent::MulticastDrawShooting_Implementation()
{
	// Draw Outer Sphere
	DrawSphere(MaximumRadius, FColor::Red);
	
	// Draw Inner Sphere
	DrawSphere(CalculateRadiusBasedOffCurrentCharge(), FColor::Green);
}

float UGrenadeWeaponComponent::CalculateCurrentChargeBasedOffMaxCharge() const
{
	/*
	 *	MaximumCharge - 1
	 *	CurrentCharge = x
	 *	MaximumCharge / CurrentCharge = 1/x
	 *	CurrentCharge / MaximumCharge = x
	*/
	return CurrentCharge / MaximumCharge;
}

float UGrenadeWeaponComponent::CalculateRadiusBasedOffCurrentCharge() const
{
	/*
	 * MaximumRaidus - 1
	 * x - CalculateCurrentChargeBasedOffMaxCharge
	 * MaximumRadius * CalculateCurrentChargeBasedOffMaxCharge = x
	*/
	return MaximumRadius * CalculateCurrentChargeBasedOffMaxCharge();
}

// Called every frame
void UGrenadeWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsGainingCharge)
	{
		return;
	}
	if (CurrentCharge <= MaximumCharge)
	{
		CurrentCharge += ChargeGainPerSecond * DeltaTime;
	}
	
}

bool UGrenadeWeaponComponent::IsCurrentlyShooting()
{
	return bIsGainingCharge;
}

void UGrenadeWeaponComponent::Multicast_DrawCustomSphereOnAllClients_Implementation(const int32 Radius,
	const FColor& Color)
{
	DrawSphere(Radius, Color);
}

void UGrenadeWeaponComponent::DrawSphere(const int32 Radius, const FColor& Color) const
{
	DrawDebugSphere(
		GetWorld(),
		GetSocketLocation(GetAttachSocketName()),
		Radius,
		32,
		Color,
		false,
		0,
		0,
		0
	);
}

