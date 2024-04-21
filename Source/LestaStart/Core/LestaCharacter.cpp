// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"

ALestaCharacter::ALestaCharacter()
{
	NetUpdateFrequency = 10.f;
	CurrentlyActiveWeaponIndex = 0;
	bShouldCycleThroughInventory = true;

	WeaponInventory = CreateDefaultSubobject<UWeaponInvenotryComponent>(TEXT("Weapon Inventory"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	MaxHP = 100.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraComponent->bUsePawnControlRotation = true; // Camera rotation is synchronized with Player Controller rotation
	CameraComponent->SetupAttachment(GetMesh());
}

void ALestaCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Устанавливает кол-во хп в рантайме потому что В таком случае установится то, что показывается в блюпринтах
	HealthComponent->SetHealth(MaxHP);
}

int32 ALestaCharacter::CycleWeaponsIndex(int32 Index) const
{
	if (bShouldCycleThroughInventory)
	{
		Index = std::abs(Index % WeaponInventory->GetInventorySize());
		
	}
	else
	{
		if (Index >= WeaponInventory->GetInventorySize())
		{
			Index = WeaponInventory->GetInventorySize() - 1;
		}
		else if (Index <= 0)
		{
			Index = 0;
		}
	}
	return std::abs(Index);
}

void ALestaCharacter::OnShootingEnded()
{
	IWeaponInterface* const CurrentlyActiveWeapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	if (CurrentlyActiveWeapon)
	{
		CurrentlyActiveWeapon->StopShooting();
	}
}

void ALestaCharacter::OnChooseFirstWeapon()
{
	if (IsShooting()) return;
	OnShootingEnded();
	CurrentlyActiveWeaponIndex = 0;
}

void ALestaCharacter::OnChooseSecondWeapon()
{
	if (IsShooting()) return;
	OnShootingEnded();
	CurrentlyActiveWeaponIndex = 1;
}

void ALestaCharacter::OnSwitchWeapons(const FInputActionValue& InputActionValue)
{
	if (IsShooting()) return;
	OnShootingEnded();
	// Returns either -1 or 1
	CurrentlyActiveWeaponIndex += static_cast<int>(InputActionValue.GetMagnitude());
	CurrentlyActiveWeaponIndex = CycleWeaponsIndex(CurrentlyActiveWeaponIndex);
}


void ALestaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC) // should validate component because developers can change input component class through Project Settings
	{
		EIC->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInput);
		EIC->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnLookInput);
		EIC->BindAction(ShootInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnShootInput);
		EIC->BindAction(ShootInputAction, ETriggerEvent::Canceled, this, &ThisClass::OnShootingEnded);
		EIC->BindAction(ShootInputAction, ETriggerEvent::Completed, this, &ThisClass::OnShootingEnded);
		EIC->BindAction(ChooseFirstWeaponInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnChooseFirstWeapon);
		EIC->BindAction(ChooseSecondWeaponInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnChooseSecondWeapon);
		EIC->BindAction(SwitchWeaponsInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnSwitchWeapons);
		
	}
	else
	{
		// Print error message into log
		// You can read more here: https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine
			UE_LOG(LogInput, Error, TEXT("Unexpected input component class: %s"), *GetFullNameSafe(PlayerInputComponent))
	}
}

bool ALestaCharacter::IsShooting() const
{
	
	return WeaponInventory->GetWeaponAt(CycleWeaponsIndex(CurrentlyActiveWeaponIndex))->IsCurrentlyShooting();
}

bool ALestaCharacter::CanHoldWeapon() const
{
	return true;
}

bool ALestaCharacter::SetWeapon(IWeaponInterface* Weapon)
{
	WeaponInventory->PushWeapon(Weapon);
	return (Weapon != nullptr);
}

float ALestaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageAmount);
	if (HealthComponent->GetHealth() < 0.f)
	{
		Destroy();
	}
	return DamageAmount;
}

void ALestaCharacter::OnMoveInput(const FInputActionInstance& InputActionInstance)
{
	// Controller rotation Yaw determines which direction Character is facing
	// so MoveForward = along that direction AND MoveRight = to the right of that direction

	const float YawDegree = GetControlRotation().Yaw; // controller rotation Yaw (in degrees)
	const float YawRadian = FMath::DegreesToRadians(YawDegree); // controller rotation Yaw (in radians)
	const FVector ForwardDirection = FVector(FMath::Cos(YawRadian), FMath::Sin(YawRadian), 0.f);
	const FVector RightDirection = FVector(FMath::Cos(YawRadian + UE_HALF_PI), FMath::Sin(YawRadian + UE_HALF_PI), 0.f);

	const FVector2D Input2D = InputActionInstance.GetValue().Get<FVector2D>();
	AddMovementInput(ForwardDirection * Input2D.X + RightDirection * Input2D.Y);
}

void ALestaCharacter::OnLookInput(const FInputActionInstance& InputActionInstance)
{
	const FVector2D Input2D = InputActionInstance.GetValue().Get<FVector2D>();
	AddControllerYawInput(Input2D.X);
	AddControllerPitchInput(Input2D.Y);
}

void ALestaCharacter::OnShootInput(const FInputActionInstance& InputActionInstance)
{
	IWeaponInterface* const CurrentlyActiveWeapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	if (CurrentlyActiveWeapon)
	{
		CurrentlyActiveWeapon->Shoot();
	}
}
