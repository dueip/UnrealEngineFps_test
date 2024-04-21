// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "DestroyableObject.generated.h"

UCLASS()
class LESTASTART_API ADestroyableObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADestroyableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Health")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Health")
	float MaxHp;
};
