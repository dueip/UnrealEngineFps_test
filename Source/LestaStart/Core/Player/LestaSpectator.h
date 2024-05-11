// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "../LestaGameState.h"
#include "LestaSpectator.generated.h"

UCLASS()
class LESTASTART_API ALestaSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALestaSpectator();

	
	virtual void PossessedBy(AController* NewController) override;
	virtual UInputComponent* CreatePlayerInputComponent() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPlayerSpectatingChanged();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnReastarting();
	
	UFUNCTION(Server, Reliable)
	virtual void ServerVoteOnRestarting();
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> RequestRestartAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> RequestChangeSpectatingPlayer;

protected:

	UFUNCTION()
	void OnRestartRequested();

private:
	bool bHasAlreadyVoted: 1 = false;
};
