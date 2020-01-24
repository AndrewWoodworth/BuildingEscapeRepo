// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	// Member variables
	float Reach = 100.f;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	FVector LineTraceEnd;

	UPROPERTY(EditAnyWhere)
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	UPROPERTY(EditAnyWhere)
	UInputComponent* InputComponent = nullptr;

	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInputComponent();
	void DrawReachDebug();

	// Return the first actor within reach with a physics body
	FHitResult GetFirstPhysicsBodyInReach();

	// Return the line trace end
	FVector GetLineTraceEnd();
};
