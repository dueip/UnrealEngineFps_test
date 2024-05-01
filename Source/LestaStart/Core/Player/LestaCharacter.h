// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "DeadPlayer.h"
#include "LestaStart/Core/HealthComponent.h"
#include "LestaStart/Core/Weapons/WeaponHoldableInterface.h"
#include "LestaStart/Core/Weapons/WeaponInvenotryComponent.h"
#include "LestaCharacter.generated.h"

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
class LESTASTART_API ALestaCharacter : public ACharacter, public IWeaponHoldableInterface
{
	GENERATED_BODY()

	// You can extend this class if required
	// Blueprint Character class is derived from the ALestaCharacter

public:
	ALestaCharacter();
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void OnDead();
	int32 CycleWeaponsIndex(int32 Index) const;


	UFUNCTION(BlueprintNativeEvent)
	void CreateHUD();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	bool IsShooting() const;
	virtual bool CanHoldWeapon() const override;
	virtual bool SetWeapon(IWeaponInterface* Weapon) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintGetter)
	int32 GetHealth() const;
	
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
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TObjectPtr<UWeaponInvenotryComponent> WeaponInventory;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 CurrentlyActiveWeaponIndex;

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

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="Stats")
	float MaxHP;
	
	
	UPROPERTY(EditDefaultsOnly, Category="Death")
	TSubclassOf<ADeadPlayer> DeadPlayerToSpawn;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnMoveInput(const FInputActionInstance& InputActionInstance);
	virtual void OnLookInput(const FInputActionInstance& InputActionInstance);
	virtual void OnShootInput(const FInputActionInstance& InputActionInstance);
	virtual void OnSwitchWeapons(const FInputActionValue& InputActionValue);
	virtual void OnShootingEnded();
	virtual void OnChooseFirstWeapon();
	virtual void OnChooseSecondWeapon();
	bool IsReloading() const;
	UFUNCTION()
	void ReloadWeapon();
	virtual void OnReload();
private:
	bool bIsDead = false;
	FTimerHandle ReloadTimerHandle;

	

	
	TEnumAsByte<EPlayerState> PlayerState;
};
