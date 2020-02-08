// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#include "DefaultCharacter.generated.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	DoorRotation = GetOwner()->GetActorRotation();
	InitialYaw = DoorRotation.Yaw;
	OpenAngle += InitialYaw;
	CurrentYaw = InitialYaw;

	if (bUsePressurePlate) {CheckForPressurePlate();}
	FindAudioComponent();

	// ----------------TEST CODE-------------------------
	if (!bUsePressurePlate)
	{
		UE_LOG(LogTemp, Warning, TEXT("The number of RotatableActors is: %i"), RotatableActors.Num());
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing an audio component!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::CheckForPressurePlate() const
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has an OpenDoor component attached, but no PressuePlate set."), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bUsePressurePlate)
	{
		CheckActorsRotations();
	}

	if (TotalMassOfActors() >= MassToOpenDoor || bRotatableActorsHaveCorrectRotation || CheckForOveralppingActorThatOpens())
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else if (CurrentYaw != InitialYaw)
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	// Print rotation to log.
	// UE_LOG(LogTemp, Warning, TEXT("Object Yaw is: %s"), *FString::Printf(TEXT("%.2f"), CurrentYaw));

	DoorRotation.Yaw = FMath::Lerp(CurrentYaw, OpenAngle, DoorOpenSpeed * DeltaTime);
	CurrentYaw = DoorRotation.Yaw;

	GetOwner()->SetActorRotation(DoorRotation);

	// Play door sound
	bCanPlayCloseDoorSound = true;
	if(!AudioComponent) {return;}
	if (bCanPlayOpenDoorSound)
	{
		AudioComponent->Play();
		bCanPlayOpenDoorSound = false;
	}
}	

void UOpenDoor::CloseDoor(float DeltaTime)
{
	DoorRotation.Yaw = FMath::Lerp(CurrentYaw, InitialYaw, DoorCloseSpeed * DeltaTime);
	CurrentYaw = DoorRotation.Yaw;

	GetOwner()->SetActorRotation(DoorRotation);

	// Play door sound
	bCanPlayOpenDoorSound = true;
	if(!AudioComponent) {return;}
	if (bCanPlayCloseDoorSound)
	{
		AudioComponent->Play();
		bCanPlayCloseDoorSound = false;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);	

	// Add up overlapping actors masses
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->IsRootComponentMovable())
		{
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}

	return TotalMass;
}

bool UOpenDoor::CheckForOveralppingActorThatOpens() const
{
	if (!PressurePlate)
	{
		return false;
	}
	
	return PressurePlate->IsOverlappingActor(ActorThatOpens);
}

void UOpenDoor::CheckActorsRotations()
{
	if (RotatableActors.Num() == -1 || !RotatableActorsRotations.IsValidIndex(0))
	{
		// ----------------TEST CODE-------------------------
		if (!bUsePressurePlate)
		{
			UE_LOG(LogTemp, Warning, TEXT("No rotatable actors!"));
		}

		return;
	}

	int32 NumCorrectRotations = 0;

	for (int32 i = 0; i < RotatableActors.Num(); i++)
	{
		FRotator ActorRotation = RotatableActors[i]->GetActorRotation();
		//UE_LOG(LogTemp, Warning, TEXT("RotatableActorsRotation of index: %i = %f; and the ActorRotation.Yaw = %f"), i, RotatableActorsRotations[i], ActorRotation.Yaw);

		if (RotatableActorsRotations[i] == FMath::RoundToFloat(FMath::Abs(ActorRotation.Yaw)))
		{
			//UE_LOG(LogTemp, Warning, TEXT("The Number of Correct Rotations is: %i"), NumCorrectRotations);
			NumCorrectRotations += 1;
			if (NumCorrectRotations >= RotatableActors.Num())
			{
				bRotatableActorsHaveCorrectRotation = true;
				// ADefaultCharacter ObjectToRotate = Cast<ADefaultCharacter>(ADefaultCharacter);
				// AActor* NewObjectToRotate = *ObjectToRotate;
				// UE_LOG(LogTemp, Warning, TEXT("%s is the ObjectToRotate"), *NewObjectToRotate->GetName());
			}
		}
		else
		{
			bRotatableActorsHaveCorrectRotation = false;
			//UE_LOG(LogTemp, Error, TEXT("bRotatableActorsHaveCorrectRotation = false"));
		}
	}
}