// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "LaserComponent.h"
#include "LaserWeaponComponent.generated.h"


DECLARE_DELEGATE(FShootDelegate)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API ULaserWeaponComponent : public UWeaponComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULaserWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void CalculateAnimationDurationAndSetTimer();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	virtual void StopShooting() override;
	
	virtual void Shoot() override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULaserComponent> Laser;

	FTimerHandle BlinkAnimationTimer;

	UPROPERTY()
	FColor BaseColor;
	UPROPERTY(EditAnywhere, Category="Animation")
	FColor BlinkColor;
	UPROPERTY(EditAnywhere, Category="Animation")
	float BlinkDuration;
	UPROPERTY(EditAnywhere, Category="Animation")
	float AnimationDuration;

	FShootDelegate ShootDelegate;

	UPROPERTY(EditAnywhere, Category="Damage")
	double LaserLength;
	UPROPERTY(EditAnywhere, Category="Damage")
	float DamageAmount;

	UFUNCTION()
	void BlinckingAnimationCallback();
};

	

