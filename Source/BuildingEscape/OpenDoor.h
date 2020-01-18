// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"

#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);

	// Member Variables

	float DoorLastOpened = 0.f;
	float InitialYaw;
	float CurrentYaw;
	FRotator DoorRotation;
	
	UPROPERTY(EditAnyWhere)
	float OpenAngle = 90.f;
	
	UPROPERTY(EditAnyWhere)
	ATriggerVolume* PressurePlate;

	UPROPERTY(EditAnyWhere)
	AActor* ActorThatOpens;

	UPROPERTY(EditAnyWhere)
	float DoorOpenSpeed = 0.5f;

	UPROPERTY(EditAnyWhere)
	float DoorCloseSpeed = 1.3f;

	UPROPERTY(EditAnyWhere)
	float DoorCloseDelay = 0.4f;
};
