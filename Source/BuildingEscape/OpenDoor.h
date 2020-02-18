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

	// Public Functions
	void CheckActorsRotations(float DeltaTime);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	bool CheckForOveralppingActorThatOpens() const;
	float TotalMassOfActors() const;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	void CheckForPressurePlate() const;
	void FindAudioComponent();
	void UpdateMatArray(int32 IndexOfArray);
	void ChangeMaterial(float NewMaterialMetalness, class UMaterialInstanceDynamic* Material, FName NameOfBlendParamter, float DeltaTime);
	void CheckForRotatableActorMat() const;
	void FillMatInstDynamicArray();

	// Member Variables
	bool bCanPlayCloseDoorSound = false;
	bool bCanPlayOpenDoorSound = true;
	bool bRotatableActorsHaveCorrectRotation = false;
	float CurrentYaw;
	float DoorLastOpened = 0.f;
	float DoorLastClosed = 0.f;
	float InitialYaw;
	float CurrentMetalness = 0.f;
	FRotator DoorRotation;

	UPROPERTY(EditAnyWhere)
	bool bUsePressurePlate = true;

	UPROPERTY(EditAnyWhere)
	bool bUseRotatableActors = false;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	bool bIsSecondDoor = false;

	UPROPERTY(EditAnyWhere)
	AActor* ActorThatOpens = nullptr;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUsePressurePlate"))
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnyWhere)
	float DoorCloseDelay = 0.3f;

	UPROPERTY(EditAnyWhere)
	float DoorOpenDelay = 0.3f;
	
	UPROPERTY(EditAnyWhere)
	float DoorCloseSpeed = 1.5f;

	UPROPERTY(EditAnyWhere)
	float DoorOpenSpeed = 0.5f;

	UPROPERTY(EditAnyWhere)
	float OpenAngle = 90.f;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUsePressurePlate"))
	float MassToOpenDoor = 50.f;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	TArray<float> RotatableActorsRotations;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	TArray<AActor*> RotatableActors;
	
	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	class UMaterial* RotatableActorMat = nullptr;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	TArray<UMaterialInstanceDynamic*> MaterialInstDynamicArray;

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	FName NameOfBlendParamter = TEXT("MetalBlendAmount");

	UPROPERTY(EditAnyWhere, meta=(EditCondition="bUseRotatableActors"))
	int32 MaterialIndex = 0;

	UPROPERTY()
	UStaticMeshComponent* ChangeMatMesh = nullptr;

	UPROPERTY(EditAnyWhere)
	UAudioComponent* AudioComponent = nullptr;
};
