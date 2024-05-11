// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "IMessageTracer.h"
#include "ScreenPass.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameSession.h"
#include "LestaStart/Core/LestaGameMode.h"
#include "Net/UnrealNetwork.h"


bool ATurret::CanRecieveDamageFromFriendlies() const
{
	return bCanFriendlyFire;
}

void ATurret::ReceiveDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (!CanRecieveDamageFromFriendlies() && dynamic_cast<ThisClass*>(DamageCauser))
	{
		return;
	}
	TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, Health);
	DOREPLIFETIME(ATurret, CurrentMode);
}

// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BeginningMode = Modes::None;
	ViewRadius = 500.f;
	FOV = 2 * UE_PI;
	bCanFriendlyFire = false;
	ScoutingRotationSpeed = 500.f;
	RotationSpeedWhenAttacking = ScoutingRotationSpeed * 1.5f;
	MaxHP = 0;
	ActorLockedOnto = nullptr;

	TimeBetweenShots = 1.f;
	
	SetReplicates(true);
	SetReplicatingMovement(true);
	
	if (!Health)
	{
		Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
		
	}
	Health->HealthChangedDelegate.AddUFunction(this, FName("OnHealthChanged"));

	USceneComponent* Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	USphereComponent* Vision = CreateDefaultSubobject<USphereComponent>(TEXT("VisionRange"));
	Vision->SetupAttachment(Scene);
	Vision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnEnteredView);
	Vision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnExitedView);
	
	Vision->SetSphereRadius(ViewRadius);

	VisionSphere = Vision;
	
	bReplicates = true;
	
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	// Set the mode to the current
	Health->SetHealth(MaxHP);
	CurrentMode = BeginningMode;
	if (CurrentMode == Modes::None)
	{
		CurrentMode = Modes::Scouting;
	}
	bReplicates = true;
}

void ATurret::ChangeStateTo(const Modes Mode)
{
	CurrentMode = Mode;
}

float ATurret::GetDistanceToActor(FHitResult& InHitResult, const TObjectPtr<AActor> Actor)
{
	


	return 0;
}

bool ATurret::CheckIfHitWasTheSameActor(const TObjectPtr<const AActor> Actor, const FHitResult& Hit)
{
	return Actor == Hit.GetActor();
}

bool ATurret::CheckIfViewToActorIsBlocked(const TObjectPtr<AActor> Actor) const
{
	FHitResult Hit;
	const FVector TurretLocation = GetActorLocation();
	const FVector TraceEnd = Actor->GetActorLocation();
	bool bBlockHit = GetWorld()->LineTraceSingleByChannel(Hit, TurretLocation, TraceEnd, ECC_Pawn);
	return !(CheckIfHitWasTheSameActor(Actor, Hit) && Hit.bBlockingHit); 
}

bool ATurret::CheckIfActorIsInTheFOV(const TObjectPtr<AActor> Actor, bool bShouldIgnoreSelf) const
{
	FHitResult Hit;
	const FVector TurretLocation = GetActorLocation();
	const FVector TraceEnd = Actor->GetActorLocation();
	bool bBlockHit = GetWorld()->LineTraceSingleByChannel(Hit, TurretLocation, TraceEnd, ECC_Pawn);
	const float DistanceFromTheTurret = FVector::Distance(TurretLocation, TraceEnd);
	bool bIsInTheFov = DistanceFromTheTurret <= VisionSphere->GetScaledSphereRadius();
	bool bWasThereAHit = false;
	bWasThereAHit =  CheckIfHitWasTheSameActor(Actor, Hit) && Hit.bBlockingHit; 
	return bWasThereAHit && bIsInTheFov;
}

FRotator ATurret::InterpolateToActorsLocation(const FVector& ActorLocation, const float RotationSpeed) const
{
	// Расстояние между Турелью и Павном 
	FVector DirectionVector = GetActorLocation() - ActorLocation;
	DirectionVector.Normalize();
	
	float Pitch = FMath::Atan2(DirectionVector.Z,
		FMath::Sqrt(FMath::Square(DirectionVector.X) + FMath::Square(DirectionVector.Y)));
	float Yaw = FMath::Atan2(DirectionVector.Y, DirectionVector.X);

	//FRotator работает с градусами (очень странно?)
	Pitch = FMath::RadiansToDegrees(Pitch);
	Yaw = FMath::RadiansToDegrees(Yaw);

	// Добавляем 180 градусов к Yaw чтобы просто повернуть на 180 градусов, потому что иначе турель смотрит ровно в
	// другую сторону
	const FRotator EndRotation(Pitch, Yaw + 180, GetActorRotation().Roll);
			
	const FRotator CurrentRotation = GetActorRotation();
	// Интерполируем чтобы был красивый поворот, а не просто телепорт 
	return FMath::RInterpTo(CurrentRotation, EndRotation,
	                                        GetWorld()->GetDeltaSeconds(), RotationSpeed);
}


void ATurret::DrawFOV()
{
	FColor DebugCircleColor(255, 0, 0, 125);
	DrawDebugCircle(
		GetWorld(),
		GetActorLocation(),
		VisionSphere->GetScaledSphereRadius(),
		32,
		DebugCircleColor,
		false,
		0,
		0,
		10,
		FVector(1, 0, 0),
		FVector(0, 1, 0),
		false);
}

void ATurret::ObjectLeftFOV()
{
	// TODO: Probably should do a better thing, but for now this also works; 
	ActorCurrentlyBeingAttacked = nullptr;
	ServerRequestChangeStateTo(Modes::Scouting);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	

	if (!GetWorld()) { return; }
	//const TArray<ULocalPlayer*>& GamePlayers = GEngine->GetGamePlayers(GetWorld());
	APlayerController* Player = GetWorld()->GetFirstPlayerController();
	
	
	DrawFOV();	
	
	switch (CurrentMode)
	{
	case Modes::Scouting:
		{
			FRotator DeltaRotator = FRotator(0, UE_PI / 2, 0) * DeltaTime * ScoutingRotationSpeed; 
			AddActorWorldRotation(DeltaRotator);

			if (!ActorCurrentlyBeingAttacked && ActorLockedOnto)
			{
				bool bIsOverlapping = VisionSphere->IsOverlappingActor(ActorLockedOnto);
				// Проверяемся на то, что мы "нашли" актора, но он уже вышел из-за стену уву!
				if (bIsOverlapping && !CheckIfViewToActorIsBlocked(ActorLockedOnto))
				{
					ActorCurrentlyBeingAttacked = ActorLockedOnto;
					ChangeStateTo(Modes::Activated);
				}
				
			}
			
			break;
		}
	case Modes::Activated:
		// Если актор не за стеной, то всё ок, начинаем анимацию
		if (ActorCurrentlyBeingAttacked && !AnimationTimerHandle.IsValid())
		{
			GetWorldTimerManager().SetTimer(AnimationTimerHandle, this, &ThisClass::ServerRequestChangeStateToAttack, 1.f);
		}
		break;
	case Modes::Attacking:
		{
			// Если турелька не видит актора, то что-то плохо :(
			if (ActorCurrentlyBeingAttacked)
			{
				const FRotator NewRotation = InterpolateToActorsLocation(ActorCurrentlyBeingAttacked->GetActorLocation(),
					RotationSpeedWhenAttacking * DeltaTime);
				SetActorRotation(NewRotation);
				if (CheckIfViewToActorIsBlocked(ActorCurrentlyBeingAttacked))
				{
					ObjectLeftFOV();
				}
			}
		}
	}
		
}

void ATurret::ServerRequestChangeStateToAttack_Implementation()
{
	if (ActorCurrentlyBeingAttacked)
	{
		ChangeStateTo(Modes::Attacking);
	} else
	{
		ChangeStateTo(Modes::Scouting);
	}
	AnimationTimerHandle.Invalidate(); 
}


float ATurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                          AActor* DamageCauser)
{
	
	if (DamageCauser == this)
	{
		return 0.f;
	}

	
	Health->SetHealth(Health->GetHealth() - DamageAmount);
	if (Health->GetHealth() <= 0.f)
	{
		SetActorScale3D(GetActorScale3D() / 2);
		if (HasAuthority())
		{
			ServerRequestDestroy();
		}
	}
	return DamageAmount;
}

void ATurret::ServerRequestDestroy_Implementation()
{
	Destroy();
}

void ATurret::ServerRequestChangeStateTo_Implementation(const Modes Mode)
{
	CurrentMode = Mode;
}


void ATurret::OnHealthChanged(float NewHealth)
{
	
	//UE_LOG(LogTemp, Warning, TEXT("%s's new health is: %f"), GetName().GetCharArray().GetData(), NewHealth);
}

void ATurret::OnEnteredView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	//if (dynamic_cast<ACharacter*>(OtherActor))
	//{
		if (!ActorCurrentlyBeingAttacked)
		{
			ActorLockedOnto = OtherActor;
		}
		if (ActorLockedOnto)
		{
			if (!CheckIfViewToActorIsBlocked(ActorLockedOnto))
			{
				ActorCurrentlyBeingAttacked = ActorLockedOnto;
				ServerRequestChangeStateTo(Modes::Activated);
			}
		}
		
	//}
}

void ATurret::OnExitedView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == ActorLockedOnto || ActorLockedOnto == nullptr)
	{
		ActorLockedOnto = nullptr;
		ActorCurrentlyBeingAttacked = nullptr;
		ServerRequestChangeStateTo(Modes::Scouting);
	}
}

