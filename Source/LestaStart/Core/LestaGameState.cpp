// Fill out your copyright notice in the Description page of Project Settings.


#include "LestaGameState.h"

void ALestaGameState::ServerVote_Implementation(EVoteType VotedOnWhat)
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
