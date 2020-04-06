// Copyright Andrew Woodworth 2019-2020 All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/HUD.h"
#include "DefaultCharacter.h"
#include "DefaultHUD.generated.h"


UCLASS()
class BUILDINGESCAPE_API ADefaultHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	// Sets the default values for this HUD's properties.
	ADefaultHUD();

	UFUNCTION(BlueprintCallable)
	void UpdateReticle();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void LoadAssets();

private:
	// Member Variables
	FVector2D ViewportSize;

	UPROPERTY()
	class UTexture* CurrentReticleTexture = nullptr;

	UPROPERTY(EditAnyWhere, Category = Crosshair)
	class UTexture* NotInteractableReticleTexture = nullptr;

	UPROPERTY(EditAnyWhere, Category = Crosshair)
	class UTexture* InteractableReticleTexture = nullptr;

	UPROPERTY()
	class ADefaultCharacter* PlayerPtr = nullptr;
};
