// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserWeaponComponent.h"
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
	
	UPROPERTY(EditDefaultsOnly, Category="Mode")
	Modes BeginningMode;	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeStateTo(const Modes Mode);
	
	float GetDistanceToPawn(FHitResult& InHitResult, const APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	bool CheckIfPawnIsInTheFOV(const APawn* Pawn) const;

	UFUNCTION(BlueprintCallable)
	FRotator InterpolateToPawnsLocation(const APawn* Pawn, float RotationSpeed) const;

	UPROPERTY(EditAnywhere, Category="Vision")
	float ViewRadius;

	UPROPERTY(EditAnywhere, Category="Vision")
	// Should somehow display that as degrees?
	float FOV;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float ScoutingRotationSpeed;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle TimerHandle;

	void ChangeStateToAttack() {  ChangeStateTo(Modes::Attacking); TimerHandle.Invalidate(); } ;
	Modes CurrentMode;
};
