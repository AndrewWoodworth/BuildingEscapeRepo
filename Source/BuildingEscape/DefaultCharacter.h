// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

UCLASS()
class BUILDINGESCAPE_API ADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AActor* ObjectToRotate = nullptr;

	// Sets default values for this character's properties
	ADefaultCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveBackward(float Value);
	void MoveLeft(float Value);

	void Interact();
	void Grab();
	void ReleaseGrabbed();
	void CheckForObjectsToRotate();
	void RotateObjects(float DeltaTime);

private:
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
};
