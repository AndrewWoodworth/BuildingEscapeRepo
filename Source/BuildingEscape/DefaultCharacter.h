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

	void Grab();
	void Release();

	// Return the line trace end
	FVector GetLineTraceEnd();

private:
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	FVector LineTraceEnd;

	float TurnSpeed = 45.f;
	float LookUpSpeed = 45.f;

	UPROPERTY(EditAnyWhere)
	float PlayerMass = 60.f;

	UPROPERTY(EditAnyWhere)
	float Reach = 200.f;

	UPROPERTY()
	USceneComponent* GrabTransform = nullptr;

	UPROPERTY()
	class UPhysicsHandleComponent* PhysicsHandle = nullptr;
};
