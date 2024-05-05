﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../HealthComponent.h"
#include "Turret.generated.h"


UENUM()
enum class Modes
{
	None = 0	UMETA(Hidden), 
	Alarm		UMETA(DisplayName="Alarm"),
	Activated	UMETA(DisplayName="Activated"),
	Attacking	UMETA(DisplayName="Attacking"),
	Deactivated UMETA(DisplayName="Deactivated"),
	Scouting	UMETA(DisplayName="Scouting")
};

UCLASS()
class LESTASTART_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	//DECLARE_DELEGATE_RetVal(FCheckIfPawnIsInTheFOV);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category="Mode")
	Modes BeginningMode;
	UPROPERTY(EditAnywhere, Category="Animation")
	float RotationSpeedWhenAttacking;
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void ServerRequestDestroy();
	
	UFUNCTION(Server, Reliable)
	void ServerRequestChangeStateTo(const Modes Mode);
	
	UFUNCTION(BlueprintCallable)
	void ChangeStateTo(const Modes Mode);
	
	float GetDistanceToActor(FHitResult& InHitResult, const TObjectPtr<AActor> Actor);
	static bool CheckIfHitWasTheSameActor(const TObjectPtr<AActor> Actor, const FHitResult& Hit);

	bool CheckIfActorIsInTheFOV(const TObjectPtr<AActor> Actor) const;

	FRotator InterpolateToActorsLocation(const TObjectPtr<AActor> Actor, float RotationSpeed) const;
	void DrawFOV();

	UPROPERTY(EditAnywhere, Category="Vision")
	float ViewRadius;

	UPROPERTY(EditAnywhere, Category="Vision")
	// Should somehow display that as degrees?
	float FOV;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float ScoutingRotationSpeed;

	UFUNCTION()
	virtual void OnShoot() {};
	UFUNCTION()
	virtual void OnStopShooting() {};
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/*
		Timers
	*/
	FTimerHandle AnimationTimerHandle;
	FTimerHandle TimerBetweenShotsHandle;

	UFUNCTION(Server, Reliable)
	void ServerRequestChangeStateToAttack();
	void destr()
	{
		Destroy();
	}

	UPROPERTY(Replicated)
	Modes CurrentMode;

	UPROPERTY(EditAnywhere, Category="Health")
	float MaxHP;
	
	UPROPERTY(EditDefaultsOnly, Category="Health", Replicated)
	TObjectPtr<UHealthComponent> Health;

	UPROPERTY(EditAnywhere, Category="Attack")
	float TimeBetweenShots;
	
	UFUNCTION(Blueprintable)
	void OnHealthChanged(float NewHealth);


	UPROPERTY(EditAnywhere, Replicated) 
	TSubclassOf<AActor> Target;
private:

};
