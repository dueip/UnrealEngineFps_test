// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserComponent.h"


// Sets default values for this component's properties
ULaserComponent::ULaserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

	const UWorld* const World =  GetWorld();
	const AActor* Owner = GetOwner();
	if (IsValid(World) && IsValid(Owner))
	{
		const FVector OwnerPosition = Owner->GetTransform().GetLocation();
		const FVector BeginOffset = FVector(0, 000, 50);
		const FVector EndOffset = BeginOffset + FVector(500, 0, 0);
		DrawDebugLine(World, OwnerPosition + BeginOffset, OwnerPosition + EndOffset, FColor::Emerald);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("World or Owner is invalid"));
	}
}

