// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "InterchangeResult.h"
#include "GameFramework/GameSession.h"
#include "Net/UnrealNetwork.h"
#include "Settings/LevelEditorPlayNetworkEmulationSettings.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true;
	HealthPoints = 0.f;
	SetIsReplicatedByDefault(true);
	// ...
}



// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	//HealthPoints = HealthPoints;
	//SetHealth(HealthPoints);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// ...
}

void UHealthComponent::SetHealth(int32 NewHP)
{
	HealthPoints = NewHP;
	if (HealthChangedDelegate.IsBound())
	{
		HealthChangedDelegate.Broadcast(NewHP);
	}
	MulticastEnsureAllClientsKnowThatHealthChanged(NewHP);
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, HealthPoints);
}

void UHealthComponent::ServerSetHealth_Implementation(int32 NewHP)
{
	
	HealthPoints = NewHP;
	MulticastEnsureAllClientsKnowThatHealthChanged(NewHP);	
}

void UHealthComponent::MulticastEnsureAllClientsKnowThatHealthChanged_Implementation(int32 NewHP)
{
	if (HealthChangedDelegate.IsBound())
	{
		HealthChangedDelegate.Broadcast(NewHP);
	}
}

void UHealthComponent::OnRep_HP(int32 NewHP)
{
	MulticastEnsureAllClientsKnowThatHealthChanged(NewHP);
	return;
}

