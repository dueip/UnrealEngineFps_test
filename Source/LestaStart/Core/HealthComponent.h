﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "HealthComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FHealthChangedDelegate, float);

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

	//UFUNCTION(Server, Unreliable)
	void SetHealth(float NewHP);
	UFUNCTION(BlueprintGetter)
	float GetHealth() const { return HealthPoints;};

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastEnsureAllClientsKnowThatHealthChanged(float NewHP);

	UFUNCTION(Server, Unreliable)
	void ServerSetHealth(float NewHP);
	
	FHealthChangedDelegate HealthChangedDelegate;
protected:
	
	UPROPERTY(EditAnywhere, Category="Health", ReplicatedUsing=OnRep_HP)
	float HealthPoints;

	UFUNCTION()
	void OnRep_HP();
};
