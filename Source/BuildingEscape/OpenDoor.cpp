// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialExpressionDynamicParameter.h"

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

	DefaultCharacterPtr = Cast<ADefaultCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	DoorRotation = GetOwner()->GetActorRotation();
	InitialYaw = DoorRotation.Yaw;
	OpenAngle += InitialYaw;
	CurrentYaw = InitialYaw;

	if (bUsePressurePlate) {CheckForPressurePlate();}
	FindAudioComponent();
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

	// TODO: Should a pressure plate and rotatable actors be allowed to open the door at the same time?
	if (!bUsePressurePlate)
	{
		CheckActorsRotations(DeltaTime);
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
	if (!PressurePlate) {return false;}
	return PressurePlate->IsOverlappingActor(ActorThatOpens);
}

void UOpenDoor::CheckActorsRotations(float DeltaTime)
{
	if (RotatableActors.Num() == -1 || !RotatableActorsRotations.IsValidIndex(0)) {return;}

	int32 NumCorrectRotations = 0;
	if (!DefaultCharacterPtr || !DefaultCharacterPtr->ObjectToRotate || ChangeMatMesh) {return;}
	ChangeMatMesh = DefaultCharacterPtr->ObjectToRotate->FindComponentByClass<UStaticMeshComponent>();
	MaterialInstDynamic = UMaterialInstanceDynamic::Create(ActorCorrectRotationMat, ChangeMatMesh);
	ChangeMatMesh->SetMaterial(MaterialIndex, MaterialInstDynamic);

	for (int32 i = 0; i < RotatableActors.Num(); i++)
	{
		if (RotatableActorsRotations[i] == FMath::RoundToFloat(FMath::Abs(RotatableActors[i]->GetActorRotation().Yaw)))
		{
			NumCorrectRotations += 1;
			ChangeMaterial(1.f, MaterialInstDynamic, NameOfBlendParamter, ChangeMatMesh, DeltaTime);
			if (NumCorrectRotations >= RotatableActors.Num())
			{
				bRotatableActorsHaveCorrectRotation = true;
			}
		}
		else
		{
			bRotatableActorsHaveCorrectRotation = false;
			ChangeMaterial(0.f, MaterialInstDynamic, NameOfBlendParamter, ChangeMatMesh, DeltaTime);
		}
	}
}

void UOpenDoor::ChangeMaterial(float MaterialMetalness, class UMaterialInstanceDynamic* MaterialInstDynamic, FName NameOfBlendParamter, UStaticMeshComponent* MeshToChangeMatOf, float DeltaTime)
{
	if (MaterialInstDynamic && MeshToChangeMatOf)
	{
		MaterialInstDynamic->GetScalarParameterValue(FMaterialParameterInfo(NameOfBlendParamter), CurrentMetalness);

		CurrentMetalness = FMath::Lerp(CurrentMetalness, MaterialMetalness, 0.8f * DeltaTime);

		MaterialInstDynamic->SetScalarParameterValue(NameOfBlendParamter, CurrentMetalness);
	}
}