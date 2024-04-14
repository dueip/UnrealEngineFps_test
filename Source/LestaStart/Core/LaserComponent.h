// Fill out your copyright notice in the Description page of Project Settings.

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

	UFUNCTION(BlueprintCallable)
	virtual void SetOrigin(const FVector& NewBeginPosition);
	UFUNCTION(BlueprintCallable)
	virtual void SetEndPosition(const FVector& NewEndPosition);
	UFUNCTION(BlueprintCallable)
	virtual void SetColor(const FColor& NewColor);
	/* Moves both Begin and End positions by MoveVector*/
	UFUNCTION(BlueprintCallable)
	virtual void Move(const FVector& MoveVector);


protected:
	UPROPERTY(EditDefaultsOnly)
	FVector BeginPosition;
	UPROPERTY(EditDefaultsOnly)
	FVector EndPosition;
	UPROPERTY(EditAnywhere)
	FColor Color;
	UPROPERTY(EditAnywhere)
	float Thickness;
};
