// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "DeadPlayer.h"
#include "LestaStart/Core/HealthComponent.h"
#include "LestaStart/Core/Renderers/LaserComponent.h"
#include "LestaStart/Core/Weapons/WeaponHoldableInterface.h"
#include "LestaStart/Core/Weapons/WeaponInvenotryComponent.h"
#include "Blueprint/UserWidget.h"
#include "LestaStart/Core/Weapons/DamagableInterface.h"
#include "LestaCharacter.generated.h"


#define WIDGET_DECLARE(WidgetName) \
	UPROPERTY(EditDefaultsOnly, Category="UI")\
	TSubclassOf<UUserWidget> WidgetName; \
	UPROPERTY()\
	TObjectPtr<UUserWidget> WidgetName ## _generated;

#define WIDGET_ADD_TO_HUD(widget_name)  { if (widget_name)\
	{ \
		widget_name ## _generated = CreateWidget<UUserWidget>(GetWorld(), widget_name); \
		widget_name ## _generated->AddToPlayerScreen(); \
	} else { \
	UE_LOG(LogTemp, Warning, TEXT("Widget %s could not be added to the HUD"), #widget_name); } }

#define WIDGET_REMOVE_FROM_HUD(widget_name)  { if (widget_name ## _generated) \
	{ \
		widget_name ## _generated ->RemoveFromRoot(); \
		widget_name ## _generated->Destruct(); \
		widget_name ## _generated = nullptr; \
	} }

UENUM()
enum class EPlayerState
{
	None = 0 UMETA(Hidden),
	Reloading = 1,
	Dead
};

class UCameraComponent;

/** Base Character class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaCharacter
	:
public ACharacter,
public IWeaponHoldableInterface,
public IDamagableInterface
{
	GENERATED_BODY()

	// You can extend this class if required
	// Blueprint Character class is derived from the ALestaCharacter

public:
	
	virtual void ReceiveDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	virtual bool CanRecieveDamageFromFriendlies() const override;
	
	UFUNCTION(Client, Reliable)
	void ClientTestCase();
	
	ALestaCharacter();
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void OnDead();
	int32 CycleWeaponsIndex(int32 Index) const;

	UFUNCTION(Server, Unreliable)
	void Server_DealDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser);

	UFUNCTION(Blueprintable)
	void CreateHUD();

	
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDrawShootOnAllClients(UObject* Weapon);

	bool IsShooting() const;
	virtual bool CanHoldWeapon() const override;
	virtual bool SetWeapon(IWeaponInterface* Weapon) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	FString GetHealthText() const;
	
	UFUNCTION(BlueprintGetter)
	int32 GetHealth() const;

	UFUNCTION(Server, Reliable)
	void ServerOnDead();

	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	FString GetWeaponName() const;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	float GetElapsedTimeSinceStartedReloading() const;
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	float GetCurrentWeaponReloadTime() const;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	int32 MaxWeaponAmmo() const;
	UFUNCTION(BlueprintCallable, Category="Weapon")
	float CurrentWeaponAmmo() const;

	void RequestSpawnSpectator();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Inventory", Replicated)
	TObjectPtr<UWeaponInvenotryComponent> WeaponInventory;

	UPROPERTY(EditDefaultsOnly, Category="Inventory", Replicated)
	int32 CurrentlyActiveWeaponIndex;
	/*
	 * INPUT
	 */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	bool bShouldCycleThroughInventory;
	/** Input action assigned to movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveInputAction;

	/** Input action assigned to camera movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ChooseFirstWeaponInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ChooseSecondWeaponInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SwitchWeaponsInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ReloadInputAction;
	/*
	 * HEALTH
	 */
	UPROPERTY(EditDefaultsOnly, Category="Stats", Replicated)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="Stats")
	float MaxHP;

	/*
	 * UI	
	 */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> WeaponInfoWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> WeaponInfoWidget_generated;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> StatsWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> StatsWidget_generated;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HudInfoWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> HudInfoWidget_generated;

	UPROPERTY(EditDefaultsOnly, Category="Health")
	bool bCanFriendlyFire;

	UFUNCTION(Client, Reliable)
	virtual void ClientRemoveHUD();
	/*
	 * INPUT CALLBACKS
	 */
	virtual void OnMoveInput(const FInputActionInstance& InputActionInstance);
	virtual void OnLookInput(const FInputActionInstance& InputActionInstance);
	virtual void OnShootInput(const FInputActionInstance& InputActionInstance);
	virtual void OnSwitchWeapons(const FInputActionValue& InputActionValue);
	virtual void OnShootingEnded();
	virtual void OnChooseFirstWeapon();
	virtual void OnChooseSecondWeapon();
	virtual void OnReload();

	UFUNCTION()
	void OnRep_HealthComponent(int32 NewHP);	
	UFUNCTION()
	void ReloadWeapon();
	bool IsReloading() const;
	
	UFUNCTION()
	void OnRep_DesiredRotationChanged(const FVector2D& NewRotation);
	UPROPERTY(ReplicatedUsing=OnRep_DesiredRotationChanged)
	FVector2D UpdatedLookVector;
	
	UPROPERTY(EditDefaultsOnly, Category="Death")
	TSubclassOf<ADeadPlayer> DeadPlayerToSpawn;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Unreliable)
	void ClientCalculateDesiredEndPoint(ULaserWeaponComponent* LaserWeapon);
	
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	TObjectPtr<class ULaserWeaponComponent> LaserWeaponTry;
	
	UFUNCTION(BlueprintNativeEvent)
	void TEST_FUNCTION();
	//UPROPERTY(EditDefaultsOnly)
	//ULaserComponent* JustForTesting;
private:
	FTimerHandle ReloadTimerHandle;

	
	bool bIsDead = false;
	TEnumAsByte<EPlayerState> PlayerState;
};
