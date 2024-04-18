﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DELEGATE_RetVal_OneParam(float, FHealthChangedDelegate, float)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintSetter)
	void SetHealth(float NewHP);
	UFUNCTION(BlueprintGetter)
	float GetHealth() const { return HealthPoints;};
	
	FHealthChangedDelegate HealthChangedDelegate;
private:
	UPROPERTY(EditAnywhere, Category="Health")
	float HealthPoints; 
};
