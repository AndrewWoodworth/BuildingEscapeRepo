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
	if (RotatableActors.Num() != -1 && RotatableActorMat)
	{
		MaterialInstDynamicArray.Init(nullptr, RotatableActors.Num());
		for (int32 i = 0; i < RotatableActors.Num(); i++)	
		{
			if (RotatableActors[i])
			{
				ChangeMatMesh = RotatableActors[i]->FindComponentByClass<UStaticMeshComponent>();
				if (!ChangeMatMesh) {return;}
				MaterialInstDynamicArray[i] = UMaterialInstanceDynamic::Create(RotatableActorMat, ChangeMatMesh);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *MaterialInstDynamicArray[i]->GetName());
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
	if (RotatableActors.Num() == -1 || !RotatableActorsRotations.IsValidIndex(0) || MaterialInstDynamicArray.Num() == -1 || !RotatableActorMat) {return;}

	int32 NumCorrectRotations = 0;
	for (int32 i = 0; i < RotatableActors.Num(); i++)
	{
		ChangeMatMesh = RotatableActors[i]->FindComponentByClass<UStaticMeshComponent>();

		// TODO: possibly make this into a function
		if (MaterialInstDynamicArray[i] && ChangeMatMesh)
		{
			if (ChangeMatMesh->GetMaterial(MaterialIndex) != MaterialInstDynamicArray[i])
			{
				MaterialInstDynamicArray[i] = UMaterialInstanceDynamic::Create(RotatableActorMat, ChangeMatMesh);
				ChangeMatMesh->SetMaterial(MaterialIndex, MaterialInstDynamicArray[i]);
				UE_LOG(LogTemp, Warning, TEXT("ChangeMatMesh Material is: %s"), *ChangeMatMesh->GetMaterial(MaterialIndex)->GetName());
				UE_LOG(LogTemp, Warning, TEXT("MaterialInstDynamicArray[i] is: %s"), *MaterialInstDynamicArray[i]->GetName());
			}
		}
		
		if (FMath::Abs(RotatableActorsRotations[i]) == FMath::RoundToFloat(FMath::Abs(RotatableActors[i]->GetActorRotation().Yaw)))
		{
			NumCorrectRotations += 1;
			ChangeMaterial(1.f, MaterialInstDynamicArray[i], NameOfBlendParamter, DeltaTime);
			if (NumCorrectRotations >= RotatableActors.Num())
			{
				bRotatableActorsHaveCorrectRotation = true;
			}
		}
		else
		{
			bRotatableActorsHaveCorrectRotation = false;
			ChangeMaterial(0.f, MaterialInstDynamicArray[i], NameOfBlendParamter, DeltaTime);
		}
	}
}

void UOpenDoor::ChangeMaterial(float NewMaterialMetalness, class UMaterialInstanceDynamic* Material, FName NameOfBlendParamter, float DeltaTime)
{
	if (Material)
	{

		float MatMetalness;

		Material->GetScalarParameterValue(FMaterialParameterInfo(NameOfBlendParamter), CurrentMetalness);

		MatMetalness = FMath::Lerp(CurrentMetalness, NewMaterialMetalness, 0.8f * DeltaTime);
		//CurrentMetalness = MatMetalness;

		//CurrentMetalness += MaterialMetalness*0.1f - CurrentMetalness*0.5f;

		Material->SetScalarParameterValue(NameOfBlendParamter, MatMetalness);	

		//UE_LOG(LogTemp, Warning, TEXT("The statue metalness is: %f"), CurrentMetalness);
		//UE_LOG(LogTemp, Warning, TEXT("The mat metalness is: %f"), MatMetalness);
		UE_LOG(LogTemp, Warning, TEXT("The statue's material is: %s"), *ChangeMatMesh->GetMaterial(0)->GetName());
		UE_LOG(LogTemp, Warning, TEXT("The dynamic mat inst is: %s"), *Material->GetName());
		UE_LOG(LogTemp, Warning, TEXT("The ActorCorrectRotationMat is: %s"), *RotatableActorMat->GetName());
	}
}