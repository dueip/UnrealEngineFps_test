// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "LestaStart/Core/Renderers/LaserComponent.h"
#include "optional"
#include "Engine/NetDriver.h"
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
	//FVector CalculateDesiredEndPoint()
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float GetReloadTime() override;

	virtual void DrawShooting() override;
	
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

	
	UPROPERTY(Replicated)
	FVector DesiredEndPoint;



	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DrawOnFire();
	UFUNCTION(Server, Unreliable)
	void Server_DoHitWithoutOrigin(const FVector& EndPoint, ECollisionChannel DesiredHitCollisionChannel);
	UFUNCTION(Server, Unreliable)
	void Server_DoHit(const FVector& OriginPoint, const FVector& EndPoint, ECollisionChannel DesiredHitCollisionChannel);

	

	UFUNCTION(Server, Unreliable)
	virtual void ServerShoot() override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerShootAt(const FVector& Origin, const FVector& EndPoint) override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerReload() override;
	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulticastDrawShooting() override;
	UFUNCTION(Server, Unreliable)
	virtual void ServerStopShooting() override;
	UFUNCTION(Server, Unreliable)
	void Server_DrainAmmo(int32 NumberOfAmmo);
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	TObjectPtr<ULaserComponent> Laser;
protected:

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
	int32 DurabilityLossInOneClick;

	UPROPERTY(EditAnywhere, Category="Ammo")
	float ReloadTime;
	
	UPROPERTY(EditAnywhere, Category="Damage")
	double LaserLength;
	UPROPERTY(EditAnywhere, Category="Damage")
	float DamageAmount;

	UPROPERTY(EditAnywhere, Category="Damage")
	TEnumAsByte<ECollisionChannel> HitCollisionChannel;
	
	FShootDelegate ShootDelegate;

	UFUNCTION(Server, Reliable)
	void Server_TryToUpdateDurability(float NewDrainage);


	virtual FCompletelyDrainedDelegate* GetCompletelyDrainedDelegate() override
	{
		return &CompletelyDrainedDelegate;
	}

	virtual FStartedReloadingDelegate* GetStartedReloadingDelegate() override
	{
		return &StartedReloadingDelegate;
	}

	FCompletelyDrainedDelegate CompletelyDrainedDelegate;
	FStartedReloadingDelegate StartedReloadingDelegate;


private:
	void CalculateLaserPosition(const FVector& EndPoint);


	
	UFUNCTION()
	void OnShoot();

	UFUNCTION()
	void BlinckingAnimationCallback();


	std::optional<FVector> LastHitPointAfterCollision;
	
	UPROPERTY(Replicated)
	float CurrentDurability;
};

	

