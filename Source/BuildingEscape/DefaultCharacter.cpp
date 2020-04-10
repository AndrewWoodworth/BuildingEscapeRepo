// Copyright Andrew Woodworth 2019-2020 All Rights Reserved.


#include "DefaultCharacter.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#define OUT

// Sets default values
ADefaultCharacter::ADefaultCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	GrabTransform = CreateDefaultSubobject<USceneComponent>(TEXT("GrabPosition"));

	GrabTransform->SetupAttachment(RootComponent);
	GrabTransform->SetRelativeLocation(FVector(Reach, 0.f, 70.f));
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Fill the ObjectsToRotate array with empty structs.
	ObjectsToRotate.Insert(FObjectToRotate(), ObjectsToRotate.Num());
}

// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the PhysicsHandle is attached, move and rotate the PhysicsHandle's target location and target rotation (basically move grabbed object).
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocationAndRotation(GetLineTraceEnd(), GrabTransform->GetComponentRotation());
	}

	RotateObjects(DeltaTime);
}

// Called to bind functionality to input
void ADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ADefaultCharacter::Interact);
}

void ADefaultCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADefaultCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADefaultCharacter::MoveBackward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(-GetActorForwardVector(), Value);
	}
}

void ADefaultCharacter::MoveLeft(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(-GetActorRightVector(), Value);
	}
}

void ADefaultCharacter::Interact()
{
	if (!PhysicsHandle->GrabbedComponent)
	{
		CheckForObjectsToRotate();
		Grab();
	}
	else if (PhysicsHandle->GrabbedComponent)
	{
		ReleaseGrabbed();
	}
}

void ADefaultCharacter::Grab()
{
	// Trace a line from center of players viewport and get the HitResult of the line trace if there is one.
	FCollisionQueryParams TraceParams(NAME_None, false, this);
	FHitResult HitResult;

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	ActorToGrab = HitResult.GetActor();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if (ActorToGrab && PhysicsHandle && ComponentToGrab)
	{
		ComponentToGrab->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GrabTransform->SetWorldRotation(ComponentToGrab->GetComponentRotation());
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, HitResult.BoneName, GetLineTraceEnd(), ComponentToGrab->GetComponentRotation());
	}
}

void ADefaultCharacter::ReleaseGrabbed()
{
	PhysicsHandle->GrabbedComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	PhysicsHandle->ReleaseComponent();
}

FVector ADefaultCharacter::GetLineTraceEnd()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

void ADefaultCharacter::CheckForObjectsToRotate()
{
	FCollisionQueryParams TraceParams(NAME_None, false, this);
	FHitResult HitResult;

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel2),
		TraceParams
	);

	AActor* ActorHit = HitResult.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("%i"), ObjectsToRotate.Num());
	int32 CountOfActorsNotEqual = 0;
	for (int32 i = 0; i < ObjectsToRotate.Num(); i++)
	{
		if (!ObjectsToRotate[i].bIsRotating && ActorHit)
		{
			if (ActorHit == ObjectsToRotate[i].ActorToRotate)
			{
				CountOfActorsNotEqual = 0;
				ObjectsToRotate[i].ActorRotation = ObjectsToRotate[i].ActorToRotate->GetActorRotation();
				ObjectsToRotate[i].OriginalActorYaw = ObjectsToRotate[i].ActorRotation.Yaw;
				ObjectsToRotate[i].TargetRotation = ObjectsToRotate[i].OriginalActorYaw + AmountToRotateObject;
				ObjectsToRotate[i].bIsRotating = true;
			}
			else if (CountOfActorsNotEqual >= ObjectsToRotate.Num() - 1)
			{
				FObjectToRotate ObjectToRotateStruct;
				ObjectToRotateStruct.ActorToRotate = ActorHit;
				ObjectToRotateStruct.ActorRotation = ObjectToRotateStruct.ActorToRotate->GetActorRotation();
				ObjectToRotateStruct.OriginalActorYaw = ObjectToRotateStruct.ActorRotation.Yaw;
				ObjectToRotateStruct.TargetRotation = ObjectToRotateStruct.OriginalActorYaw + AmountToRotateObject;
				ObjectToRotateStruct.bIsRotating = true;
				ObjectsToRotate.Emplace(ObjectToRotateStruct);
			}
			else
			{
				CountOfActorsNotEqual += 1;
			}
		}
		if (ActorHit == ObjectsToRotate[i].ActorToRotate && ObjectsToRotate[i].bIsRotating
		&& FMath::RoundToFloat(ObjectsToRotate[i].ActorRotation.Yaw) != FMath::RoundToFloat(ObjectsToRotate[i].OriginalActorYaw))
		{
			ObjectsToRotate[i].TargetRotation += AmountToRotateObject;
		}
	}
}

void ADefaultCharacter::RotateObjects(float DeltaTime)
{
	// Loop through all the rotatable actors, lerp their rotations, and set their rotations.
	for (int32 i = 0; i < ObjectsToRotate.Num(); i++)
	{
		if (ObjectsToRotate.Num() != -1 && ObjectsToRotate[i].bIsRotating)
		{
			// Lerp the actor's rotation.
			ObjectsToRotate[i].ActorRotation.Yaw = FMath::Lerp(ObjectsToRotate[i].ActorRotation.Yaw, ObjectsToRotate[i].TargetRotation, 1.6f * DeltaTime);

			// Set the actor's rotation.
			ObjectsToRotate[i].ActorToRotate->SetActorRotation(ObjectsToRotate[i].ActorRotation);

			// Snap actor's rotation so lerp doesn't go continuously.
			if (ObjectsToRotate[i].TargetRotation - ObjectsToRotate[i].ActorRotation.Yaw < 0.4f)
			{
				ObjectsToRotate[i].ActorRotation.Yaw = ObjectsToRotate[i].TargetRotation;
				ObjectsToRotate[i].ActorToRotate->SetActorRotation(ObjectsToRotate[i].ActorRotation);
				ObjectsToRotate[i].bIsRotating = false;
			}
		}
	}
}