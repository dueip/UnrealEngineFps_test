// Fill out your copyright notice in the Description page of Project Settings.


#include "DeleteThisLater.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ADeleteThisLater::ADeleteThisLater()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	LaserComponent = CreateDefaultSubobject<ULaserComponent>(TEXT("Laser"));
}

// Called when the game starts or when spawned
void ADeleteThisLater::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ADeleteThisLater::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ADeleteThisLater, LaserComponent);
}

// Called every frame
void ADeleteThisLater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LaserComponent->SetOrigin(GetActorLocation());
	LaserComponent->SetEndPoint(GetActorLocation() + FVector(100, 0, 0));
	
	LaserComponent->MulticastDrawOnAllClients();
}

