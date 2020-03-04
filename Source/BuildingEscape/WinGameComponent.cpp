// Copyright Andrew Woodworth 2019-2020 All Rights Reserved


#include "WinGameComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/Color.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

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

	FirstPlayerController = GetWorld()->GetFirstPlayerController();
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

	if (WinGameTriggerVolume->IsOverlappingActor(ActorThatWins) && bCanLoadWinLevel)
	{
		bCanLoadWinLevel = false;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.f, 1.f, 2.f, FLinearColor(0.f, 0.f, 0.f, 1.f), false, true);
		GetWorld()->GetTimerManager().SetTimer(FadeScreenTimerHandle, this, &UWinGameComponent::LoadWinLevel, 2.f, false);
	}
}

void UWinGameComponent::LoadWinLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("WinScreenLevel"), false);
}