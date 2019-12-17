// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"

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

	TargetYaw = -90.f;
	FRotator OpenDoor = FRotator(0.f, 0.f, 0.f);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogTemp, Warning, TEXT("Object Yaw is: %f"), GetOwner()->GetActorRotation().Yaw);
	
	float StartingYaw = GetOwner()->GetActorRotation().Yaw;
	OpenDoor.Yaw = FMath::Lerp(StartingYaw, TargetYaw, 0.02f);

	GetOwner()->SetActorRotation(OpenDoor);
}

