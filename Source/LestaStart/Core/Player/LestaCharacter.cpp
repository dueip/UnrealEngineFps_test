// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LestaPlayerController.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Camera/CameraComponent.h"
#include "LestaStart/Core/HealthComponent.h"
#include "LestaStart/Core/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Core/Weapons/WeaponInterface.h"
#include "LestaStart/Core/Weapons/WeaponInvenotryComponent.h"

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

void ALestaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ULaserWeaponComponent* LaserWeapon = FindComponentByClass<ULaserWeaponComponent>())
	{
		// Assuming LaserWeapon and CameraComponent are valid and accessible
		FVector CameraCenter = CameraComponent->GetComponentLocation();
		FVector CameraForward = CameraComponent->GetForwardVector();

		// Calculate the endpoint by extending the camera's forward direction by the laser's length
		FVector DesiredEndPoint = CameraCenter + (CameraForward * LaserWeapon->GetLaserLength());
		// Now set the DesiredEndPoint of the LaserWeapon
		LaserWeapon->DesiredEndPoint = DesiredEndPoint;
	}
}

void ALestaCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Устанавливает кол-во хп в рантайме потому что В таком случае установится то, что показывается в блюпринтах
	HealthComponent->SetHealth(MaxHP);
	CreateHUD();
	
}

void ALestaCharacter::OnDead()
{
	OnShootingEnded();	
	if (DeadPlayerToSpawn)
	{
		ADeadPlayer* DeadPlayer = GetWorld()->SpawnActor<ADeadPlayer>(DeadPlayerToSpawn,
			GetActorLocation(), GetActorRotation());
		GetController()->Possess(DeadPlayer);
		DeadPlayer->AfterPossesed();
		//DeadPlayer->SetupPlayerInputComponent(GetController()->InputComponent);
	}
	
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	bIsDead = true;
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

void ALestaCharacter::CreateHUD_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Please implement"));
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
	if (IsReloading()) return;
	if (IsShooting()) return;
	OnShootingEnded();
	CurrentlyActiveWeaponIndex = 0;
	OnShootingEnded();
}
	
void ALestaCharacter::OnChooseSecondWeapon()
{
	if (IsReloading()) return;
	if (IsShooting()) return;
	OnShootingEnded();
	CurrentlyActiveWeaponIndex = 1;
	OnShootingEnded();
}

bool ALestaCharacter::IsReloading() const
{
	return GetWorldTimerManager().IsTimerActive(ReloadTimerHandle);
}

void ALestaCharacter::ReloadWeapon()
{
	IWeaponInterface* Weapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	Weapon->Reload();
}


void ALestaCharacter::OnReload()
{
	IWeaponInterface* Weapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	if (Weapon)
	{
		if (!IsReloading())
		{
			GetWorldTimerManager().SetTimer
			(
				ReloadTimerHandle,
				this,
				&ThisClass::ReloadWeapon,
				Weapon->GetReloadTime()
			);

		}
	}
}

void ALestaCharacter::OnSwitchWeapons(const FInputActionValue& InputActionValue)
{
	if (IsReloading()) return;
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
		EIC->BindAction(ReloadInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnReload);
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
		OnDead();
	}
	return DamageAmount;
}

int32 ALestaCharacter::GetHealth() const
{
	return (HealthComponent->GetHealth() < 0.f ? 0 : HealthComponent->GetHealth()) ;
}

FString ALestaCharacter::GetWeaponName() const
{
	return WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex)->GetDisplayName().ToString();
}

float ALestaCharacter::GetElapsedTimeSinceStartedReloading() const
{
	if (IsReloading())
	{
		return GetWorldTimerManager().GetTimerElapsed(ReloadTimerHandle);
	}
	return 0.f;
	
}

float ALestaCharacter::GetCurrentWeaponReloadTime() const
{
	return WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex)->GetReloadTime();
}

int32 ALestaCharacter::MaxWeaponAmmo() const
{
	return WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex)->GetMaxDrainage();
}

float ALestaCharacter::CurrentWeaponAmmo() const
{
	float CurrentDrainage = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex)->GetCurrentDrainage();
	return CurrentDrainage <= 0.f ? 0 : CurrentDrainage;
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
		if (IsReloading()) { CurrentlyActiveWeapon->StopShooting(); return;}
		// Можем стрелялть только если оружие нам разрешает (т.е. в большинстве случаев у него есть патроны)
		if (!CurrentlyActiveWeapon->IsDrained())
			CurrentlyActiveWeapon->Shoot();
		else
			CurrentlyActiveWeapon->StopShooting();
	}
}
