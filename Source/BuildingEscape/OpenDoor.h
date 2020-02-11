// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include "DefaultCharacter.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool CheckForOveralppingActorThatOpens() const;
	float TotalMassOfActors() const;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	void CheckForPressurePlate() const;
	void FindAudioComponent();
	void CheckActorsRotations(float DeltaTime);
	void ChangeMaterial(int32 MaterialIndex, class UMaterial* NewMaterial, class UMaterial* OldMaterial, UStaticMeshComponent* MeshToChangeMatOf, float DeltaTime);

	// Member Variables
	bool bCanPlayCloseDoorSound = false;
	bool bCanPlayOpenDoorSound = true;
	bool bRotatableActorsHaveCorrectRotation = false;
	float CurrentYaw;
	float DoorLastOpened = 0.f;
	float InitialYaw;
	FRotator DoorRotation;
	TArray<UObject*> DefaultSubobjects;

	UPROPERTY(EditAnyWhere)
	AActor* ActorThatOpens = nullptr;

	UPROPERTY(EditAnyWhere)
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnyWhere)
	bool bUsePressurePlate = true;

	UPROPERTY()
	class ADefaultCharacter* DefaultCharacterPtr;
	
	UPROPERTY(EditAnyWhere)
	class UMaterial* StatueInCorrectRotationMat = nullptr;

	UPROPERTY(EditAnyWhere)
	class UMaterial* StatueNotInCorrectRotationMat = nullptr;

	UPROPERTY(EditAnyWhere)
	float DoorCloseDelay = 0.3f;
	
	UPROPERTY(EditAnyWhere)
	float DoorCloseSpeed = 1.5f;

	UPROPERTY(EditAnyWhere)
	float DoorOpenSpeed = 0.5f;

	UPROPERTY(EditAnyWhere)
	float MassToOpenDoor = 50.f;

	UPROPERTY(EditAnyWhere)
	float OpenAngle = 90.f;

	UPROPERTY(EditAnyWhere)
	FName NameOfMeshToChangeMatFor = TEXT("StaticMeshComponent0");

	UPROPERTY(EditAnyWhere)
	TArray<float> RotatableActorsRotations;

	UPROPERTY(EditAnyWhere)
	TArray<AActor*> RotatableActors;

	UPROPERTY(EditAnyWhere)
	UAudioComponent* AudioComponent = nullptr;
};
