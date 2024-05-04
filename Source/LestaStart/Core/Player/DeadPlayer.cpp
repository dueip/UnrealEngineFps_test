// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADeadPlayer::ADeadPlayer()
{
	InputMappingPriority = 10.f;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADeadPlayer::BeginPlay()
{
	Super::BeginPlay();


}

void ADeadPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetController()))
	{
		if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->RemoveMappingContext(InputMapping);
		}
	}
}

void ADeadPlayer::AfterPossesed()
{

	if (!HasAuthority())
	{
		return;
	}
	
	if (ULocalPlayer* LocalPlayer = GWorld->GetFirstLocalPlayerFromController())
	{
		if (auto* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSystem->AddMappingContext(InputMapping, InputMappingPriority);
		}
	}
	if (UInputComponent* IC =  GetController()->InputComponent)
	{
		IC->ClearBindingValues();
	}
	SetupPlayerInputComponent(GetController()->InputComponent);
}

// Called every frame
void ADeadPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



// Called to bind functionality to input
void ADeadPlayer::SetupPlayerInputComponent_Implementation(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC) // should validate component because developers can change input component class through Project Settings
	{
		EIC->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnLookInput);
		EIC->BindAction(RestartInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnRestartInput);
	}
	else
	{
		// Print error message into log
		// You can read more here: https://dev.epicgames.com/documentation/en-us/unreal-engine/logging-in-unreal-engine
		UE_LOG(LogInput, Error, TEXT("Unexpected input component class: %s"), *GetFullNameSafe(PlayerInputComponent))
	}
}

void ADeadPlayer::OnRestartInput()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetMapName()), true);
}

void ADeadPlayer::OnLookInput(const FInputActionInstance& InputActionInstance)
{
	const FVector2D Input2D = InputActionInstance.GetValue().Get<FVector2D>();
	AddControllerYawInput(Input2D.X);
	AddControllerPitchInput(Input2D.Y);
}

