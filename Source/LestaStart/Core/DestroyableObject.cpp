// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyableObject.h"


// Sets default values
ADestroyableObject::ADestroyableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	MaxHp = 100.f;
	
}

// Called when the game starts or when spawned
void ADestroyableObject::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->SetHealth(MaxHp);
	
}

// Called every frame
void ADestroyableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADestroyableObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageAmount);
	if (HealthComponent->GetHealth() < 0.f)
	{
		Destroy();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
}



