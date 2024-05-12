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
	virtual ASpectatorPawn* SpawnSpectatorPawn() override;

	virtual void ChangeState(FName NewState) override;
	virtual void ClientGotoState_Implementation(FName NewState) override;
	
	virtual void ServerViewNextPlayer_Implementation() override;
	
	UFUNCTION(Server, Reliable)
	void ServerVoteForRestart();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	/** Added input mapping context on startup. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<ALestaSpectator> SpectatorToSpawn;
	
	/** Priority of InputMapping. */
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (ClampMin = 0))
	int32 InputMappingPriority = 0;

	
	UPROPERTY(Replicated)
	bool bWantsToSpectate = false;

	bool bHasAlreadyVoted = false;

	virtual void BeginPlay() override;
};
