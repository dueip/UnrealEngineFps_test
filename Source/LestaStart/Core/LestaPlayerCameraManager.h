// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LestaPlayerCameraManager.generated.h"

/** Base Player Camera Manager class for the Lesta Start project. */
UCLASS()
class LESTASTART_API ALestaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	// You can extend this class if required
	// Blueprint PlayerCameraManager class is derived from the ALestaPlayerCameraManager
};
