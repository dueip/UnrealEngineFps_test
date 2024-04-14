// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserComponent.h"

#include "Evaluation/IMovieSceneEvaluationHook.h"


// Sets default values for this component's properties
ULaserComponent::ULaserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Thickness = 10.f;
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
		DrawDebugLine(World, OwnerPosition + BeginOffset, OwnerPosition + EndOffset, Color,
			true, -1, 0, Thickness);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("World or Owner is invalid"));
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

void ULaserComponent::Move(const FVector& MoveVector)
{
	BeginPosition += MoveVector;
	EndPosition += MoveVector;
}

