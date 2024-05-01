// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyableObject.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADestroyableObject::ADestroyableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	MaxHp = 100.f;
	bAlwaysRelevant = true;
	bReplicates = true;
	
}

// Called when the game starts or when spawned
void ADestroyableObject::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->SetHealth(MaxHp);
	
	
}

void ADestroyableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADestroyableObject, HealthComponent);
}

// Called every frame
void ADestroyableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//auto* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	//SetOwner(LocalPlayerController);
//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("health is: %f"), HealthComponent->GetHealth()));
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Authority is: %i"), HasNetOwner()));
}

float ADestroyableObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HealthComponent->SetHealth(HealthComponent->GetHealth() - DamageAmount);
	if (HealthComponent->GetHealth() < 0.f)
	{
		OnDead();
		
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
}

void ADestroyableObject::OnDead()
{
	Destroy();
}

void ADestroyableObject::MulticastOnDead_Implementation()
{
	
	OnDead();
}



