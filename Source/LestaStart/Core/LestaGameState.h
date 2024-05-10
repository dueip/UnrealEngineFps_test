// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LestaGameState.generated.h"


UENUM()
enum class EVoteType
{
	RestartGame,
	KickPlayer
};

/**
 * 
 */
UCLASS()
class LESTASTART_API ALestaGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	int32 GetVotedOnRestart() const { return VotedOnRestartCount; };

	UFUNCTION(Server, Reliable)
	void ServerVote(EVoteType VotedOnWhat);
private:
	int32 VotedOnRestartCount;
};
