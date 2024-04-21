// Fill out your copyright notice in the Description page of Project Settings.


#include "HPRenderComponent.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UHPRenderComponent::UHPRenderComponent()
{
	PlaceholderForHP = FText::FromString("{HP}");
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SetText( FText::FromString(MakeActualStringFromTemplate(0)));
	
	// ...
}

FString UHPRenderComponent::MakeActualStringFromTemplate(const float NewHP) const
{
	FText CopyTemplate = TextTemplate;
	FString Title = CopyTemplate.ToString().Replace(
		GetData(PlaceholderForHP.ToString()),
		GetData(FText::AsNumber(NewHP).ToString()),
		ESearchCase::CaseSensitive);
	return Title;
}

void UHPRenderComponent::OnHealthChanged(float NewHP)
{
	
	SetText(FText::FromString(MakeActualStringFromTemplate(NewHP)));
}


// Called when the game starts
void UHPRenderComponent::BeginPlay()
{
	Super::BeginPlay();
	//Text = FText::FromString(DisplayTitle.ToString() + ": 0");
	// It's fine to assume here that we have an HP component inside of the actor
	// Because who would ever use HPRendererComponent if there is no HP?
	const AActor* const OuterActor = dynamic_cast<AActor*>(GetOuter());
	if (OuterActor)
	{
		UHealthComponent* HealthComponent = dynamic_cast<UHealthComponent*>(
			OuterActor->FindComponentByClass(UHealthComponent::StaticClass())
			);
		if (HealthComponent)
		{
			HealthComponent->HealthChangedDelegate.AddUFunction(this, FName("OnHealthChanged"));
		}
	}
	// ...
}


// Called every frame
void UHPRenderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

