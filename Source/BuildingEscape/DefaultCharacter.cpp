// Copyright Andrew Woodworth 2019-2020 All Rights Reserved.


#include "DefaultCharacter.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UObject/ConstructorHelpers.h"

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

	//LoadAssets();
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = Cast<AHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the PhysicsHandle is attached move and rotate the PhysicsHandle's target location and target rotation (basically move grabbed object).
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocationAndRotation(GetLineTraceEnd(), GrabTransform->GetComponentRotation());
	}

	RotateObjects(DeltaTime);
}

void ADefaultCharacter::LoadAssets()
{
	//Textures
	static ConstructorHelpers::FObjectFinder<UTexture2D> NotInteractableReticleTextureObj(
		TEXT("/Engine/ArtTools/RenderToTexture/Textures/127grey")
	);
	NotInteractableReticleTexture = NotInteractableReticleTextureObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> InteractableReticleTextureObj(
		TEXT("/Engine/EngineResources/AICON-Red")
	);
	InteractableReticleTexture = InteractableReticleTextureObj.Object;
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
	UPrimitiveComponent* ComponentHit = HitResult.GetComponent();

	if (!bIsRotating && ActorHit)
	{
		ObjectToRotate = ActorHit;
		ActorRotation = ObjectToRotate->GetActorRotation();
		OriginalActorYaw = ActorRotation.Yaw;
		TargetRotation = OriginalActorYaw + AmountToRotateObject;
		bIsRotating = true;
	}

	if (ActorHit == ObjectToRotate && bIsRotating && FMath::RoundToFloat(ActorRotation.Yaw) != FMath::RoundToFloat(OriginalActorYaw))
	{
		TargetRotation += AmountToRotateObject;
	}
}

void ADefaultCharacter::RotateObjects(float DeltaTime)
{
	if (ObjectToRotate && bIsRotating)
	{
		ActorRotation.Yaw = FMath::Lerp(ActorRotation.Yaw, TargetRotation, 1.6f * DeltaTime);

		ObjectToRotate->SetActorRotation(ActorRotation);

		if (TargetRotation - ActorRotation.Yaw < .4f)
		{
			ActorRotation.Yaw = TargetRotation;
			ObjectToRotate->SetActorRotation(ActorRotation);
			bIsRotating = false;
		}
	}
}

void ADefaultCharacter::UpdateReticle()
{
	if (CurrentReticleTexture)
	{
		PlayerHUD->DrawTexture(CurrentReticleTexture, ViewportSize.X / 2, ViewportSize.Y / 2, 2.0f, 2.0f, 0, 0, 0, 0);
	}

	FCollisionQueryParams TraceParams(NAME_None, false, this);
	FHitResult HitResult;

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		PlayerViewPointLocation,
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel2),
		TraceParams
	);

	if (InteractableReticleTexture && NotInteractableReticleTexture)
	{
		AActor* ActorHit = HitResult.GetActor();
		if (ActorHit)
		{
			CurrentReticleTexture = InteractableReticleTexture;
		}
		else
		{
			CurrentReticleTexture = NotInteractableReticleTexture;
		}
	}
}