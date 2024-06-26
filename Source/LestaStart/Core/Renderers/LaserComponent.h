﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaserComponent.generated.h"


/* A simple facade class for drawing a laser from point to point in the world. Just not to worry about the backend later*/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API ULaserComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULaserComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MulticastDrawOnAllClients();

	UFUNCTION()
	void OnDraw();
	UFUNCTION(BlueprintCallable)
	virtual void SetOrigin(const FVector& NewOrigin);
	UFUNCTION(BlueprintCallable)
	virtual void SetEndPoint(const FVector& NewEndPoint);
	UFUNCTION(BlueprintCallable)
	virtual void SetColor(const FColor& NewColor);
	UFUNCTION(BlueprintGetter)
	FColor GetColor() const;
	UFUNCTION(BlueprintGetter)
	float GetThickness() const;
	/* Moves both Begin and End positions by MoveVector*/
	UFUNCTION(BlueprintCallable)
	virtual void Move(const FVector& MoveVector);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Replicated)
	FVector BeginPosition;
	UPROPERTY(EditDefaultsOnly, Replicated)
	FVector EndPosition;
	UPROPERTY(EditAnywhere, Replicated)
	FColor Color;
	UPROPERTY(EditAnywhere, Replicated)
	float Thickness;
};