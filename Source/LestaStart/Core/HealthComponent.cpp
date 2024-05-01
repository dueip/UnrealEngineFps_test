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
	SetIsReplicated(true);
	//SetIsReplicatedByDefault(true);
	// ...
}



// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetHealth(HealthPoints);

	// if (!(GetOwner() && GetOwner()->HasAuthority()))
	// {
	// 	if (HealthChangedDelegate.IsBound())
	// 	{
	// 		HealthChangedDelegate.Broadcast(HealthPoints);
	// 	}
	// }
	
	//SetIsReplicated(true);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// ...
}

void UHealthComponent::SetHealth(float NewHP)
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

void UHealthComponent::ServerSetHealth_Implementation(float NewHP)
{
	
	HealthPoints = NewHP;
	MulticastEnsureAllClientsKnowThatHealthChanged(NewHP);	
}

void UHealthComponent::MulticastEnsureAllClientsKnowThatHealthChanged_Implementation(float NewHP)
{
	if (HealthChangedDelegate.IsBound())
	{
		HealthChangedDelegate.Broadcast(NewHP);
	}
}

void UHealthComponent::OnRep_HP()
{

	return;
}

