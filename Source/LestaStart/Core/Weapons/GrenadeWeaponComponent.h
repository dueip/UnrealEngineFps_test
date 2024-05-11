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
	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable)
	virtual void ServerShoot() override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerShootAt(const FVector& Origin, const FVector& EndPoint) override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerReload() override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerStopShooting() override;
	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulticastDrawShooting() override;
	
	virtual void Shoot() override;
	virtual void StopShooting() override;
	virtual bool IsAtFullCapacity() override;
	virtual void DrawShooting() override;

	virtual bool IsDrained() override;
	virtual void Reload() override;
	virtual float GetReloadTime() override;
	virtual float GetCurrentDrainage() override;
	virtual int32 GetMaxDrainage() override;

	virtual FName GetDisplayName() const override;
	TArray<FHitResult> SweepSphere(const ECollisionChannel TraceChannel) const;
	float CalculateCurrentDamageBasedOffCurrentCharge() const;
	bool IsBlockedByOtherElement(const FVector& StartPoint, const FVector& EndPoint,
	ECollisionChannel CollisionChannel, const AActor* const HitActor) const;
	UFUNCTION(Server, Unreliable)
	void DoDamage(const TArray<FHitResult>& Hits) const;
public:
	UPROPERTY(EditAnywhere, Category="Charge", Replicated)
	float MaximumCharge;
	UPROPERTY(EditAnywhere, Category="Charge", Replicated)
	float ChargeGainPerSecond;
	UPROPERTY(EditAnywhere, Category="Charge", Replicated)
	float MaximumRadius;
	UPROPERTY(EditAnywhere, Category="Damage", Replicated)
	float MaximumDamage;
	UPROPERTY(EditAnywhere, Category="Damage", Replicated)
	float MinimumDamage;
	UPROPERTY(EditAnywhere, Category="Damage", Replicated)
	bool bShouldIgnoreOuter;
	UPROPERTY(EditAnywhere, Category="Ammo", Replicated)
	int32 MaxAmmo;
	UPROPERTY(EditAnywhere, Category="Ammo", Replicated)
	float ReloadTime;

	

	UFUNCTION(BlueprintCallable)
	float CalculateCurrentChargeBasedOffMaxCharge() const;
	UFUNCTION(BlueprintCallable)
	float CalculateRadiusBasedOffCurrentCharge() const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool IsCurrentlyShooting() override;
private:
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DrawCustomSphereOnAllClients(const int32 Radius, const FColor& Color);
	
	void DrawSphere(const int32 Radius, const FColor& Color) const;

	UPROPERTY(Replicated)
	float CurrentCharge = 0;
	UPROPERTY(Replicated)
	bool bIsGainingCharge = false;
	UPROPERTY(Replicated)
	int32 CurrentAmmoNumber;
};
