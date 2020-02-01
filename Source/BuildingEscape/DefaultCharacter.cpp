// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultCharacter.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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
}

// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the PhysicsHandle is attached move and rotate the PhysicsHandle's target location and target rotation.
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocationAndRotation(GetLineTraceEnd(), GrabTransform->GetComponentRotation());
	}
}

// Called to bind functionality to input
void ADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveBackward"), this, &ADefaultCharacter::MoveBackward);
	PlayerInputComponent->BindAxis(TEXT("MoveLeft"), this, &ADefaultCharacter::MoveLeft);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Grab"), IE_Pressed, this, &ADefaultCharacter::Interact);
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
		Grab();
		RotateObject();
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

	AActor* ActorHit = HitResult.GetActor();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	if (ActorHit && PhysicsHandle && ComponentToGrab)
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

void ADefaultCharacter::RotateObject()
{
	UE_LOG(LogTemp, Error, TEXT("RotateObject is activated."));
	FHitResult HitResult;

	// Just some tests
	/* GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult2,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1),
		TraceParams2
	); */

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1),
		FCollisionQueryParams(FName(TEXT("")), false, this)
	);

	AActor* ActorHit = HitResult.GetActor();
	UPrimitiveComponent* ComponentHit = HitResult.GetComponent();

	if (!ActorHit || !ComponentHit) {return;}
	UE_LOG(LogTemp, Warning, TEXT("%s actor was hit by a ray."), *ActorHit->GetName());
	UE_LOG(LogTemp, Warning, TEXT("%s component was hit by a ray."), *ComponentHit->GetName());
	
	const FRotator AmountOfRotation = FRotator(90);
	const FRotator NewActorRotation = ActorHit->GetActorRotation() + AmountOfRotation;

	ActorHit->SetActorRotation(FQuat(NewActorRotation.Quaternion()));
}