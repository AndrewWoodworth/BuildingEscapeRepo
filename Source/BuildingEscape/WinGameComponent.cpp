// Fill out your copyright notice in the Description page of Project Settings.


#include "WinGameComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UWinGameComponent::UWinGameComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWinGameComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ActorThatWins)
	{
		ActorThatWins = GetWorld()->GetFirstPlayerController()->GetPawn();
	}

	CheckForWinGameTriggerVolume();
	CheckForWinScreenWidget();
}

void UWinGameComponent::CheckForWinGameTriggerVolume() const
{
	if (!WinGameTriggerVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has a WinGameComponent attached but no Win Game Trigger Volume set!"), *GetOwner()->GetName());
	}
}

void UWinGameComponent::CheckForWinScreenWidget() const
{
	if (!wWinScreen)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has a WinGameComponent attached but no Win Screen (widget) set!"), *GetOwner()->GetName());
	}
}


// Called every frame
void UWinGameComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!WinGameTriggerVolume) {return;}
	if (WinGameTriggerVolume->IsOverlappingActor(ActorThatWins) && wWinScreen)
	{
		WinScreen = CreateWidget<UUserWidget>(GetWorld(), wWinScreen);

		if (WinScreen)
		{
			WinScreen->AddToViewport();
		}
	}
}

