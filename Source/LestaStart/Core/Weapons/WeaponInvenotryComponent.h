// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInterface.h"
#include "WeaponInvenotryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API UWeaponInvenotryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponInvenotryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	IWeaponInterface* GetWeaponAt(const int32 Index);
	
	
	void PushWeapon(IWeaponInterface* Weapon);

	UFUNCTION(BlueprintCallable)
	int32 GetInventorySize() const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	//UPROPERTY(Replicated)
	TArray<IWeaponInterface*> Weapons;
};
