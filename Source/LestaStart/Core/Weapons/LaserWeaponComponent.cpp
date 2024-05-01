// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserWeaponComponent.h"

#include "Engine/DamageEvents.h"
#include "WeaponHoldableInterface.h"
#include "LestaStart/Core/HealthComponent.h"
#include "LestaStart/Core/Renderers/LaserComponent.h"
#include "Net/UnrealNetwork.h"


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
	LaserLength = 1000.f;
	DamageAmount = 1.f;

	MaxDurability = 10;
	DurabilityLossInOneSecond = 3;
	ReloadTime = 0.5f;
	
	HitCollisionChannel = ECC_Pawn;
	
	// ...
}


// Called when the game starts
void ULaserWeaponComponent::BeginPlay()
{
	
	
	Super::BeginPlay();
	Laser->Deactivate();
	BaseColor = Laser->GetColor();
	Reload();
	SetIsReplicated(true);
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

void ULaserWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULaserWeaponComponent, Laser)
}

float ULaserWeaponComponent::GetReloadTime()
{
	return ReloadTime;
}

int32 ULaserWeaponComponent::GetMaxDrainage()
{
	return MaxDurability;
}


float ULaserWeaponComponent::GetCurrentDrainage()
{
	return CurrentDurability;
}

std::optional<FVector> ULaserWeaponComponent::DoHit(const FVector& SocketOrigin, const FVector& EndPoint, ECollisionChannel CollisionChannel) const
{
	FHitResult Hit;
	bool bWasThereAHit = GetWorld()->LineTraceSingleByChannel(Hit, SocketOrigin, DesiredEndPoint, CollisionChannel);
	FPointDamageEvent PointDamage;
	PointDamage.Damage = DamageAmount;
	PointDamage.HitInfo = Hit;
	// Мы вполне можем получить нуллптр тут и нам не надо волноваться об этом:
	// Это просто значит то, что у нас не было оружие присоединено к Актору
	AActor* ActorThatDealtDamage = UECasts_Private::DynamicCast<AActor*>(GetOuter());
	if (bWasThereAHit && Hit.bBlockingHit)
	{
		// Если есть компонент с ХП, то значит мы можем нанести урон
		// Потом можно перенести в интерфейс
		if (UHealthComponent* Health = Hit.GetActor()->FindComponentByClass<UHealthComponent>())
		{
			//Hit.GetActor()->TakeDamage(DamageAmount, PointDamage, nullptr, ActorThatDealtDamage);
			Health->ServerSetHealth(Health->GetHealth() - DamageAmount);
		}
		
		return Hit.Location;
	}
	return std::nullopt;
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
	FVector EndPoint = DesiredEndPoint;
	Laser->SetOrigin(SocketOrigin);
	
	
	
	const std::optional<FVector> HitPointAfterCollision = DoHit(SocketOrigin, EndPoint, HitCollisionChannel);
	EndPoint = HitPointAfterCollision.value_or(EndPoint);
	
	if (IsValid(GetWorld()) && !BlinkAnimationTimer.IsValid())
	{
		CalculateAnimationDurationAndSetTimer();
	}
	Laser->SetEndPoint(EndPoint);
	
	UE_LOG(LogTemp, Warning, TEXT("Owner role: %i"), GetOwnerRole());
	Laser->MulticastDrawOnAllClients();
	
	CurrentDurability -= DurabilityLossInOneSecond * DeltaTime;
}

void ULaserWeaponComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	Laser->DestroyComponent(bDestroyingHierarchy);
}

void ULaserWeaponComponent::StopShooting()
{
	Laser->Deactivate();
}


void ULaserWeaponComponent::Shoot()
{
	Laser->Activate();
}

bool ULaserWeaponComponent::IsCurrentlyShooting()
{
	return Laser->IsActive();
}

FName ULaserWeaponComponent::GetDisplayName() const
{
	return "Laser";
}

double ULaserWeaponComponent::GetLaserLength() const
{
	return LaserLength;
}

float ULaserWeaponComponent::GetLaserThickness() const
{
	return Laser->GetThickness();
}

FVector ULaserWeaponComponent::CalculateDefaultEndPoint() const
{
	const FVector SocketOrigin = GetSocketLocation(GetAttachSocketName());
	const FRotator SocketRotation = GetSocketRotation(GetAttachSocketName());

	FVector EndPoint = SocketOrigin + FVector(
		LaserLength * cos(FMath::DegreesToRadians(SocketRotation.Yaw)),
		LaserLength * sin(FMath::DegreesToRadians(SocketRotation.Yaw)),
		LaserLength * sin(FMath::DegreesToRadians(SocketRotation.Pitch)));
	return EndPoint;
}

bool ULaserWeaponComponent::IsDrained()
{
	return CurrentDurability <= 0;
}

void ULaserWeaponComponent::Reload()
{
	CurrentDurability = MaxDurability;
}

void ULaserWeaponComponent::BlinckingAnimationCallback()
{
	Laser->SetColor(Laser->GetColor() == BaseColor ? BlinkColor : BaseColor);
	BlinkAnimationTimer.Invalidate();
}

