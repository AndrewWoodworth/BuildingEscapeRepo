// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Containers/UnrealString.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorRotation = GetOwner()->GetActorRotation();
	InitialYaw = DoorRotation.Yaw;
	TargetYaw += InitialYaw;
	CurrentYaw = InitialYaw;

	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has an open door component attached, but no PressuePlate set."), *GetOwner()->GetName());
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor(DeltaTime);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	// Print rotation to log.
	// UE_LOG(LogTemp, Warning, TEXT("Object Yaw is: %s"), *FString::Printf(TEXT("%.2f"), CurrentYaw));

	DoorRotation.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, 1.f * DeltaTime);
	CurrentYaw = DoorRotation.Yaw;

	GetOwner()->SetActorRotation(DoorRotation);
}

