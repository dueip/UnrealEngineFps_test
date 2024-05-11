// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "Weapons/DamagableInterface.h"
#include "DestroyableObject.generated.h"

UCLASS()
class LESTASTART_API ADestroyableObject : public AActor, public IDamagableInterface
{
	GENERATED_BODY()

public:
	virtual void ReceiveDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	// Sets default values for this actor's properties
	ADestroyableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnDead();

	UFUNCTION()
	void OnDead();


protected:
	UPROPERTY(EditDefaultsOnly, Category="Health", Replicated)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category="Health")
	float MaxHp;
};
