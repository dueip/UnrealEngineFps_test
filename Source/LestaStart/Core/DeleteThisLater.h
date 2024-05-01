// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Renderers/LaserComponent.h"
#include "DeleteThisLater.generated.h"

UCLASS()
class LESTASTART_API ADeleteThisLater : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADeleteThisLater();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, EditDefaultsOnly)
	ULaserComponent* LaserComponent;
	
};
