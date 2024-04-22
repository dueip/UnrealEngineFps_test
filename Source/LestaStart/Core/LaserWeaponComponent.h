﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "LaserComponent.h"
#include "LaserWeaponComponent.generated.h"


DECLARE_DELEGATE(FShootDelegate)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API ULaserWeaponComponent : public USceneComponent, public IWeaponInterface
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

	virtual bool IsCurrentlyShooting() override;

	virtual FName GetDisplayName() const override;
	
	UFUNCTION(BlueprintGetter)
	double GetLaserLength() const;

	UFUNCTION(BlueprintGetter)
	float GetLaserThickness() const;
	
	FVector CalculateDefaultEndPoint();

	FVector DesiredEndPoint;
	
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

	

