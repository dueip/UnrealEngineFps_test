// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LestaGameMode.generated.h"

/** Base Game Mode class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaGameMode : public AGameModeBase
{
	GENERATED_BODY()

	
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
public:
	
	/**
	 * Массив пользователей, которые сейчас находятся в игре.
	 * Используется массив, т.к. нам редко когда понадобится на самом деле удалять что-либо.
	 */
	UPROPERTY()
	TArray<APlayerController*> JoinedPlayers;

	UFUNCTION(BlueprintGetter)
	float GetTimeForRestart() const { return TimeForRestart; };
	
	UFUNCTION(BlueprintGetter)
	FTimerHandle& GetTravelTimerHandler() { return TravelTimer; }

	UFUNCTION(BlueprintCallable)
	int32 GetHowManyPlayersNeedToVoteOnRestart() { return GetNumPlayers() + GetNumSpectators(); } 

protected:
	UPROPERTY(EditDefaultsOnly)
	float TimeForRestart;
	UPROPERTY()
	FTimerHandle TravelTimer;
};
