// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserComponent.h"

#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
ULaserComponent::ULaserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Thickness = 5.f;
	SetIsReplicatedByDefault(true);
	// ...
	
}


// Called when the game starts
void ULaserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULaserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//MulticastDrawOnAllClients();
	
}

void ULaserComponent::MulticastDrawOnAllClients_Implementation()
{
	OnDraw();
}

void ULaserComponent::OnDraw()
{
	const UWorld* const World =  GetWorld();
    	if (IsValid(World))
    	{
    		DrawDebugLine(World, BeginPosition, EndPosition, Color,
    			false, 0, 0, Thickness);
    	}
    	else
    	{
    		UE_LOG(LogTemp, Warning, TEXT("World is invalid"));
    	}
}

void ULaserComponent::SetOrigin(const FVector& NewOrigin)
{
	BeginPosition = NewOrigin;
}

void ULaserComponent::SetEndPoint(const FVector& NewEndPoint)
{
	EndPosition = NewEndPoint;
}

void ULaserComponent::SetColor(const FColor& NewColor)
{
	Color = NewColor;
}

FColor ULaserComponent::GetColor() const
{
	return Color;
}

float ULaserComponent::GetThickness() const
{
	return Thickness;
}

void ULaserComponent::Move(const FVector& MoveVector)
{
	BeginPosition += MoveVector;
	EndPosition += MoveVector;
}

void ULaserComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULaserComponent, BeginPosition);
	DOREPLIFETIME(ULaserComponent, EndPosition);
	DOREPLIFETIME(ULaserComponent, Color);
	DOREPLIFETIME(ULaserComponent, Thickness);
}
