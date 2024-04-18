﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "IMessageTracer.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BeginningMode = Modes::None;
	ViewRadius = 500.f;
	FOV = 2 * UE_PI;
	ScoutingRotationSpeed = 500.f;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	// Set the mode to the current
	CurrentMode = BeginningMode;
	if (CurrentMode == Modes::None)
	{
		CurrentMode = Modes::Scouting;
	}
}

void ATurret::ChangeStateTo(const Modes Mode)
{
	CurrentMode = Mode;
}

float ATurret::GetDistanceToPawn(FHitResult& InHitResult, const APawn* Pawn)
{
	


	return 0;
}

bool ATurret::CheckIfPawnIsInTheFOV(const APawn* Pawn) const
{
	FHitResult Hit;
	const FVector TurretLocation = GetActorLocation();
	const FVector TraceEnd = Pawn->GetActorLocation();
	bool bBlockHit = GetWorld()->LineTraceSingleByChannel(Hit, TurretLocation, TraceEnd, ECC_Pawn);
	const float DistanceFromTheTurret = FVector::Distance(TurretLocation, TraceEnd);
	return Hit.bBlockingHit && DistanceFromTheTurret <= ViewRadius;
}

FRotator ATurret::InterpolateToPawnsLocation(const APawn* Pawn, const float RotationSpeed) const
{
	// Расстояние между Турелью и Павном 
	FVector DirectionVector = GetActorLocation() - Pawn->GetActorLocation();
	DirectionVector.Normalize();
	
	float Pitch = FMath::Atan2(DirectionVector.Z,
		FMath::Sqrt(FMath::Square(DirectionVector.X) + FMath::Square(DirectionVector.Y)));
	float Yaw = FMath::Atan2(DirectionVector.Y, DirectionVector.X);

	//FRotator работает с градусами (очень странно?)
	Pitch = FMath::RadiansToDegrees(Pitch);
	Yaw = FMath::RadiansToDegrees(Yaw);

	// Добавляем 180 градусов к Yaw чтобы просто повернуть на 180 градусов, потому что иначе турель смотрит ровно в
	// другую сторону
	const FRotator EndRotation(Pitch, Yaw + 180, 0);
			
	const FRotator CurrentRotation = GetActorRotation();
	// Интерполируем чтобы был красивый поворот, а не просто телепорт 
	return FMath::RInterpTo(CurrentRotation, EndRotation,
	                                        GetWorld()->GetDeltaSeconds(), RotationSpeed);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	 
	switch (CurrentMode)
	{
	case Modes::Scouting:
		{
			FRotator DeltaRotator = FRotator(0, UE_PI / 2, 0) * DeltaTime * ScoutingRotationSpeed; 
			AddActorWorldRotation(DeltaRotator);
			
			if (Pawn)
			{
				if (CheckIfPawnIsInTheFOV(Pawn))
				{
					ChangeStateTo(Modes::Activated);
				}
			}
			break;
		}
	case Modes::Activated:
		if (!TimerHandle.IsValid())
		{
			
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::	ChangeStateToAttack, 1.f);
		}
		break;
	case Modes::Attacking:
		{
			const float InterpolationSpeed = ScoutingRotationSpeed * DeltaTime;
			const FRotator NewRotation = InterpolateToPawnsLocation(Pawn, InterpolationSpeed);
			SetActorRotation(NewRotation);
			if (Pawn && !CheckIfPawnIsInTheFOV(Pawn))
			{
				ChangeStateTo(Modes::Scouting);
			}
		}
	}
		
}

