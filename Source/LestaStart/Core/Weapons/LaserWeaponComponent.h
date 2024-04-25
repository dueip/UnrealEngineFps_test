// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "LestaStart/Core/Renderers/LaserComponent.h"
#include "optional"
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
	virtual float GetReloadTime() override;

	virtual int32 GetMaxDrainage() override;
	virtual float GetCurrentDrainage() override;
	// Returns a Hit Location if there was a hit.
	std::optional<FVector> DoHit(const FVector& SocketOrigin, const FVector& EndPoint, ECollisionChannel CollisionChannel) const;
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
	
	FVector CalculateDefaultEndPoint() const;

	virtual bool IsDrained() override;

	virtual void Reload() override;
	
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

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 MaxDurability;

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 DurabilityLossInOneSecond;

	UPROPERTY(EditAnywhere, Category="Ammo")
	float ReloadTime;
	
	UPROPERTY(EditAnywhere, Category="Damage")
	double LaserLength;
	UPROPERTY(EditAnywhere, Category="Damage")
	float DamageAmount;

	UPROPERTY(EditAnywhere, Category="Damage")
	TEnumAsByte<ECollisionChannel> HitCollisionChannel;
	
	FShootDelegate ShootDelegate;

	
	
private:
	UFUNCTION()
	void BlinckingAnimationCallback();

	float CurrentDurability;
};

	

