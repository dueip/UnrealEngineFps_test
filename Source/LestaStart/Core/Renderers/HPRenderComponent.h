// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "HPRenderComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LESTASTART_API UHPRenderComponent : public UTextRenderComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHPRenderComponent();
	
	UFUNCTION(BlueprintCallable)
	FString MakeActualStringFromTemplate(const float NewHP) const ; 

protected:
	UFUNCTION()
	void OnHealthChanged(float NewHP);
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Appereance")
	FText PlaceholderForHP;
	UPROPERTY(EditAnywhere, Category="Appereance")
	FText TextTemplate;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
