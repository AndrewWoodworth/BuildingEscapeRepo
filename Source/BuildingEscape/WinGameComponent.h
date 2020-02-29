// Copyright Andrew Woodworth 2019-2020 All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/PlayerController.h"
#include "WinGameComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UWinGameComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWinGameComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CheckForWinGameTriggerVolume() const;
	void CheckForWinScreenWidget() const;

	UFUNCTION()
	void LoadWinLevel();

	bool bCanLoadWinLevel = true;
	FTimerHandle FadeScreenTimerHandle;

	UPROPERTY(EditAnyWhere)
	ATriggerVolume* WinGameTriggerVolume = nullptr;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	AActor* ActorThatWins = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> wWinScreen;

	UPROPERTY()
	UUserWidget* WinScreen;

	UPROPERTY()
	APlayerController* FirstPlayerController = nullptr;
};