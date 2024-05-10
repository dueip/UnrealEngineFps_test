// Fill out your copyright notice in the Description page of Project Settings.


#include "LestaGameState.h"

#include "Chaos/ChaosPerfTest.h"
#include "Net/UnrealNetwork.h"

void ALestaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALestaGameState, VotedOnRestartCount);
}

void ALestaGameState::Vote(EVoteType VotedOnWhat)
{
	switch (VotedOnWhat)
	{
	case EVoteType::RestartGame:
			++VotedOnRestartCount;
		break;
	default:
		break;
	}
}
