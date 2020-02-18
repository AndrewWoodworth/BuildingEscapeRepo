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

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	DoorRotation = GetOwner()->GetActorRotation();
	InitialYaw = DoorRotation.Yaw;
	OpenAngle += InitialYaw;
	CurrentYaw = InitialYaw;

	if (bUseRotatableActors) {CheckForRotatableActorMat();}
	FillMatInstDynamicArray();

	if (bUsePressurePlate) {CheckForPressurePlate();}
	FindAudioComponent();
}

void UOpenDoor::CheckForRotatableActorMat() const
{
	if (!RotatableActorMat)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing a Rotatable Actor Material!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FillMatInstDynamicArray()
{
	if (RotatableActors.Num() != -1 && RotatableActorMat && !bIsSecondDoor)
	{
		MaterialInstDynamicArray.Init(nullptr, RotatableActors.Num());
		for (int32 i = 0; i < RotatableActors.Num(); i++)	
		{
			if (RotatableActors[i])
			{
				ChangeMatMesh = RotatableActors[i]->FindComponentByClass<UStaticMeshComponent>();
				if (!ChangeMatMesh) {return;}
				MaterialInstDynamicArray[i] = UMaterialInstanceDynamic::Create(RotatableActorMat, ChangeMatMesh);
			}
		}
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

	if (bUseRotatableActors)
	{
		CheckActorsRotations(DeltaTime);
	}

	if (TotalMassOfActors() >= MassToOpenDoor || bRotatableActorsHaveCorrectRotation || CheckForOveralppingActorThatOpens())
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastClosed >= DoorOpenDelay)
		{
			OpenDoor(DeltaTime);
			DoorLastOpened = GetWorld()->GetTimeSeconds();
		}
	}
	else if (CurrentYaw != InitialYaw)
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
			DoorLastClosed = GetWorld()->GetTimeSeconds();
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
	if (RotatableActors.Num() == -1 || !RotatableActorsRotations.IsValidIndex(0) || MaterialInstDynamicArray.Num() == -1 || !RotatableActorMat) {return;}

	int32 NumCorrectRotations = 0;
	for (int32 i = 0; i < RotatableActors.Num(); i++)
	{
		ChangeMatMesh = RotatableActors[i]->FindComponentByClass<UStaticMeshComponent>();

		if (!bIsSecondDoor)
		{
			UpdateMatArray(i);
		}
		
		if (FMath::Abs(RotatableActorsRotations[i]) == FMath::RoundToFloat(FMath::Abs(RotatableActors[i]->GetActorRotation().Yaw)))
		{
			NumCorrectRotations += 1;
			if (!bIsSecondDoor)
			{
				ChangeMaterial(1.f, MaterialInstDynamicArray[i], NameOfBlendParamter, DeltaTime);
			}

			if (NumCorrectRotations >= RotatableActors.Num())
			{
				bRotatableActorsHaveCorrectRotation = true;
			}
		}
		else
		{
			bRotatableActorsHaveCorrectRotation = false;
			if (bIsSecondDoor) {return;}
			ChangeMaterial(0.f, MaterialInstDynamicArray[i], NameOfBlendParamter, DeltaTime);
		}
	}
}

void UOpenDoor::UpdateMatArray(int32 IndexOfArray)
{
	if (MaterialInstDynamicArray[IndexOfArray] && ChangeMatMesh && !bIsSecondDoor)
	{
		if (ChangeMatMesh->GetMaterial(MaterialIndex) != MaterialInstDynamicArray[IndexOfArray])
		{
			MaterialInstDynamicArray[IndexOfArray] = UMaterialInstanceDynamic::Create(RotatableActorMat, ChangeMatMesh);
			ChangeMatMesh->SetMaterial(MaterialIndex, MaterialInstDynamicArray[IndexOfArray]);
		}
	}
}

void UOpenDoor::ChangeMaterial(float NewMaterialMetalness, class UMaterialInstanceDynamic* Material, FName NameOfBlendParamter, float DeltaTime)
{
	if (Material)
	{

		Material->GetScalarParameterValue(FMaterialParameterInfo(NameOfBlendParamter), CurrentMetalness);

		CurrentMetalness = FMath::Lerp(CurrentMetalness, NewMaterialMetalness, 0.8f * DeltaTime);

		Material->SetScalarParameterValue(NameOfBlendParamter, CurrentMetalness);
	}
}