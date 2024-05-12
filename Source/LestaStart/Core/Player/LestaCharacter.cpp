// Fill out your copyright notice in the Description page of Project Settings.

#include "LestaCharacter.h"

#include "EnhancedInputComponent.h"
#include "LestaPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "LestaStart/Core/HealthComponent.h"
#include "LestaStart/Core/Weapons/LaserWeaponComponent.h"
#include "LestaStart/Core/Weapons/WeaponInterface.h"
#include "LestaStart/Core/Weapons/WeaponInvenotryComponent.h"
#include "Net/UnrealNetwork.h"

void ALestaCharacter::ClientTestCase_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	ALestaPlayerController* PlayerController =  dynamic_cast<ALestaPlayerController*>(GetController());
	if (PlayerController && PlayerController->IsLocalPlayerController())
	{
		PlayerController->SpawnSpectatorPawn();
		
	}
		
	
}

void ALestaCharacter::ReceiveDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Если мы можем закастить, то это значит, что наш красавец 
	if (!CanRecieveDamageFromFriendlies() && dynamic_cast<ThisClass*>(DamageCauser))
	{
			return;
	}
	TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool ALestaCharacter::CanRecieveDamageFromFriendlies() const
{
	return bCanFriendlyFire;
}

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

	//JustForTesting = CreateDefaultSubobject<ULaserComponent>(TEXT("Laser"));

	bCanFriendlyFire = true;
	bIsDead = false;
	bReplicates = true;
}


void ALestaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ALestaCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Устанавливает кол-во хп в рантайме потому что В таком случае установится то, что показывается в блюпринтах
	HealthComponent->HealthChangedDelegate.AddUFunction(this, "OnRep_HealthComponent");
	if (HasAuthority())
	{
		HealthComponent->SetHealth(MaxHP);
	}
	CreateHUD();
	
}

void ALestaCharacter::OnDead()
{

	OnShootingEnded();
	bIsDead = true;
	Destroy();
	
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

void ALestaCharacter::Server_DealDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ALestaCharacter::CreateHUD()
{
	if (!GetWorld()) return;
	if (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
	{
		return;
	}
	WIDGET_ADD_TO_HUD(StatsWidget);
	WIDGET_ADD_TO_HUD(WeaponInfoWidget);
	WIDGET_ADD_TO_HUD(HudInfoWidget);
}

void ALestaCharacter::OnShootingEnded()
{
	IWeaponInterface* const CurrentlyActiveWeapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	if (CurrentlyActiveWeapon)
	{
			CurrentlyActiveWeapon->ServerStopShooting();
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
	Weapon->ServerReload();
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

void ALestaCharacter::OnRep_HealthComponent(int32 NewHP)
{
	if (!bIsDead && HealthComponent->GetHealth() <= 0.f)
	{
		if (HasAuthority())
		{
			ServerOnDead();
		}
	}
	return;
}

void ALestaCharacter::ClientRemoveHUD_Implementation()
{
	if (!GetWorld()) return;
	if (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
	{
		return;
	}
	WIDGET_REMOVE_FROM_HUD(WeaponInfoWidget);
	WIDGET_REMOVE_FROM_HUD(StatsWidget);
	WIDGET_REMOVE_FROM_HUD(HudInfoWidget);
	CollectGarbage(RF_NoFlags);
}

void ALestaCharacter::TEST_FUNCTION_Implementation()
{
	return;
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

void ALestaCharacter::MulticastDrawShootOnAllClients_Implementation(UObject* Weapon)
{
	if (IWeaponInterface* WeaponInt = dynamic_cast<IWeaponInterface*>(Weapon))
	{
		WeaponInt->DrawShooting();
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
	if (!DamageCauser->HasAuthority())
	{
		return 0;
	}
	HealthComponent->ServerSetHealth(HealthComponent->GetHealth() - DamageAmount);

	
	
	return DamageAmount;
}

FString ALestaCharacter::GetHealthText() const
{
	return FString::FromInt(GetHealth());
}

int32 ALestaCharacter::GetHealth() const
{
	return (HealthComponent->GetHealth() < 0.f ? 0 : HealthComponent->GetHealth()) ;
}

void ALestaCharacter::ServerOnDead_Implementation()
{
	ClientTestCase();
	OnDead();
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

void ALestaCharacter::RequestSpawnSpectator()
{
	
}

void ALestaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALestaCharacter, MaxHP);
	DOREPLIFETIME(ALestaCharacter, LaserWeaponTry);
	DOREPLIFETIME(ALestaCharacter, WeaponInventory);
	DOREPLIFETIME(ALestaCharacter, CurrentlyActiveWeaponIndex);
	DOREPLIFETIME(ALestaCharacter, HealthComponent);
	DOREPLIFETIME(ALestaCharacter, bIsDead);
	//DOREPLIFETIME(ALestaCharacter, JustForTesting);
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
	if (Input2D.IsNearlyZero()) return;
	AddControllerYawInput(Input2D.X);
	const float Pitch = Input2D.Y;
	AddControllerPitchInput(Pitch);
	
}

FVector ALestaCharacter::CalculateDesiredEndPoint(ULaserWeaponComponent* LaserWeapon)
{
	// Assuming LaserWeapon and CameraComponent are valid and accessible
	FVector CameraCenter = CameraComponent->GetComponentLocation();
	FVector CameraForward = CameraComponent->GetForwardVector();

	// Calculate the endpoint by extending the camera's forward direction by the laser's length
	FVector DesiredEndPoint = CameraCenter + (CameraForward * LaserWeapon->GetLaserLength());
	// Now set the DesiredEndPoint of the LaserWeapon
	LaserWeapon->DesiredEndPoint = DesiredEndPoint;
	return DesiredEndPoint;
}


void ALestaCharacter::OnShootInput(const FInputActionInstance& InputActionInstance)
{
	IWeaponInterface* const CurrentlyActiveWeapon = WeaponInventory->GetWeaponAt(CurrentlyActiveWeaponIndex);
	if (CurrentlyActiveWeapon)
	{
		if (IsReloading())
		{
		 return;
		}

		if (CurrentlyActiveWeapon->IsDrained())
		{
			return;
		}
		
		// Можем стрелялть только если оружие нам разрешает (т.е. в большинстве случаев у него есть патроны)
		ULaserWeaponComponent* LaserWeapon = FindComponentByClass<ULaserWeaponComponent>();
		const bool bIsCurrentWeaponLaser = LaserWeapon && (CurrentlyActiveWeapon == LaserWeapon);

		USceneComponent* WeaponComp = dynamic_cast<USceneComponent*>(CurrentlyActiveWeapon);
		const FVector Origin = WeaponComp->GetSocketLocation(WeaponComp->GetAttachSocketName());
		FVector EndPoint = Origin;

		if (bIsCurrentWeaponLaser)
		{
			EndPoint = CalculateDesiredEndPoint(LaserWeapon);
		}
		
		if (CurrentlyActiveWeapon->IsHitscan())
		{
			CurrentlyActiveWeapon->ServerShootAt(Origin, EndPoint);
		} else
		{
			CurrentlyActiveWeapon->ServerShoot();	
		}
	}
}
