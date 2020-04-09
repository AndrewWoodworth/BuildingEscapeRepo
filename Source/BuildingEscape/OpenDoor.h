// Copyright Andrew Woodworth 2019-2020 All Rights Reserved


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
	void LerpMaterial(float NewMaterialMetalness, class UMaterialInstanceDynamic* Material, FName NameOfBlendParamter, float DeltaTime);
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

	UPROPERTY(EditAnyWhere, Category = "Rotatable Actors")
	bool bUseRotatableActors = false;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"))
	bool bIsSecondDoor = false;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	AActor* ActorThatOpens = nullptr;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUsePressurePlate"))
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	float DoorCloseDelay = 0.3f;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	float DoorOpenDelay = 0.3f;
	
	UPROPERTY(EditAnyWhere, Category = "Optional")
	float DoorCloseSpeed = 1.5f;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	float DoorOpenSpeed = 0.5f;

	UPROPERTY(EditAnyWhere, Category = "Optional")
	float OpenAngle = 90.f;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUsePressurePlate"), Category = "Optional")
	float MassToOpenDoor = 50.f;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	TArray<float> RotatableActorsRotations;

	UPROPERTY(EditAnyWhere, meta=  (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	TArray<AActor*> RotatableActors;
	
	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	class UMaterial* RotatableActorMat = nullptr;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	TArray<UMaterialInstanceDynamic*> MaterialInstDynamicArray;

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	FName NameOfBlendParamter = TEXT("MetalBlendAmount");

	UPROPERTY(EditAnyWhere, meta = (EditCondition = "bUseRotatableActors"), Category = "Rotatable Actors")
	int32 MaterialIndex = 0;

	UPROPERTY()
	UStaticMeshComponent* ChangeMatMesh = nullptr;

	UPROPERTY(EditAnyWhere)
	UAudioComponent* AudioComponent = nullptr;
};
