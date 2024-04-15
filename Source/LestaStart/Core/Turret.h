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
	
	UPROPERTY(EditAnywhere, Category="Vision")
	float ViewRadius;

	UPROPERTY(EditAnywhere, Category="Vision")
	// Should somehow display that as degrees?
	float FOV;
	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle TimerHandle;

	void ChangeStateToAttack() { UE_LOG(LogTemp,Warning, TEXT("Changed to attack?")); ChangeStateTo(Modes::Attacking);} ;
	Modes CurrentMode;
};
