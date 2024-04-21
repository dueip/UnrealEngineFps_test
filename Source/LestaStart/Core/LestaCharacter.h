// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "LaserComponent.h"
#include "WeaponInvenotryComponent.h"
#include "WeaponInterface.h"
#include "WeaponHoldableInterface.h"
#include "LestaCharacter.generated.h"

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
	int32 CycleWeaponsIndex(int32 Index) const;
	void OnShootingEnded();
	void OnChooseFirstWeapon();
	void OnChooseSecondWeapon();
	void OnSwitchWeapons(const FInputActionValue& InputActionValue);
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	bool IsShooting() const;
	virtual bool CanHoldWeapon() const override;
	virtual bool SetWeapon(IWeaponInterface* Weapon) override;

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
	
	virtual void OnMoveInput(const FInputActionInstance& InputActionInstance);
	virtual void OnLookInput(const FInputActionInstance& InputActionInstance);
	virtual void OnShootInput(const FInputActionInstance& InputActionInstance);
};
