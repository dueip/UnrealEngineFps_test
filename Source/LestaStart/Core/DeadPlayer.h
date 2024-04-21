// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "DeadPlayer.generated.h"

UCLASS()
class LESTASTART_API ADeadPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADeadPlayer();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RestartInputAction;
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	int32 InputMappingPriority;
	void AfterPossesed();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnRestartInput();
	virtual void OnLookInput(const FInputActionInstance& InputActionInstance);
};
