// Copyright Andrew Woodworth 2019-2020 All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Templates/SubclassOf.h"
#include "DefaultCharacter.generated.h"

UCLASS()
class BUILDINGESCAPE_API ADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Public Functions
	void Interact();

	// Public variables
	UPROPERTY(BlueprintReadWrite)
	AActor* ObjectToRotate = nullptr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveBackward(float Value);
	void MoveLeft(float Value);

	
	void Grab();
	void ReleaseGrabbed();
	void CheckForObjectsToRotate();
	void RotateObjects(float DeltaTime);

private:
	// Member Variables
	bool bIsRotating = false;
	bool bCanBeGrabbing = false;
	float TargetRotation;
	float OriginalActorYaw;
	FRotator ActorRotation;
	FRotator PlayerViewPointRotation;
	FVector PlayerViewPointLocation;
	FVector LineTraceEnd;

	float TurnSpeed = 45.f;
	float LookUpSpeed = 45.f;

	UPROPERTY(EditAnyWhere)
	float AmountToRotateObject = 90.f;

	UPROPERTY(EditAnyWhere)
	float PlayerMass = 60.f;

	UPROPERTY(EditAnyWhere)
	float Reach = 200.f;

	UPROPERTY()
	USceneComponent* GrabTransform = nullptr;

	UPROPERTY()
	class UPhysicsHandleComponent* PhysicsHandle = nullptr;

	// Return the ending point for line-tracing
	FVector GetLineTraceEnd();

	// UPROPERTY()
	// TSubclassOf<AHUD>* PlayerHUD = nullptr;
};
