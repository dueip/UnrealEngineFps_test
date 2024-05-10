// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LestaSpectator.h"
#include "LestaPlayerController.generated.h"

class UInputMappingContext;

/** Base Player Controller class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaPlayerController : public APlayerController
{
	GENERATED_BODY()

	// You can extend this class if required
	// Blueprint PlayerController class is derived from the ALestaPlayerController
public:
	UFUNCTION()
	ALestaSpectator* SpawnSpectatorPawnDifferent();
	UFUNCTION()
	virtual ASpectatorPawn* SpawnSpectatorPawn() override;

	UFUNCTION(Server, Reliable)
	void ServerVoteForRestart();
	
protected:
	/** Added input mapping context on startup. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<ALestaSpectator> SpectatorToSpawn;
	
	/** Priority of InputMapping. */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (ClampMin = 0))
	int32 InputMappingPriority = 0;

	virtual void BeginPlay() override;
};
