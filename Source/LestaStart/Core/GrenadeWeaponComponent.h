// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponInterface.h"
#include "GrenadeWeaponComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API UGrenadeWeaponComponent : public USceneComponent, public IWeaponInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UGrenadeWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Shoot() override;
	virtual void StopShooting() override;

	TArray<FHitResult> SweepSphere(const ECollisionChannel TraceChannel) const;
	float CalculateCurrentDamageBasedOffCurrentCharge() const;
	bool IsBlockedByOtherElement(const FVector& StartPoint, const FVector& EndPoint,
	ECollisionChannel CollisionChannel, const AActor* const HitActor) const;
	void DoDamage(const TArray<FHitResult>& Hits) const;
public:
	UPROPERTY(EditAnywhere, Category="Charge")
	float MaximumCharge;
	UPROPERTY(EditAnywhere, Category="Charge")
	float ChargeGainPerSecond;
	UPROPERTY(EditAnywhere, Category="Charge")
	float MaximumRadius;
	UPROPERTY(EditAnywhere, Category="Damage")
	float MaximumDamage;
	UPROPERTY(EditAnywhere, Category="Damage")
	float MinimumDamage;
	UPROPERTY(EditAnywhere, Category="Damage")
	bool bShouldIgnoreOuter;

	UFUNCTION(BlueprintCallable)
	float CalculateCurrentChargeBasedOffMaxCharge() const;
	UFUNCTION(BlueprintCallable)
	float CalculateRadiusBasedOffCurrentCharge() const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool IsCurrentlyShooting() override;
private:
	float CurrentCharge = 0;
	bool bIsGainingCharge = false;
};
