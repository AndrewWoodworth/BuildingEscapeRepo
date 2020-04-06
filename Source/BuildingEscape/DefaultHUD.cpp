// Copyright Andrew Woodworth 2019-2020 All Rights Reserved


#include "DefaultHUD.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

#define OUT

ADefaultHUD::ADefaultHUD()
{
	LoadAssets();
}

void ADefaultHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine != 0)
	{
		ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	}

	PlayerPtr = Cast<ADefaultCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void ADefaultHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultHUD::LoadAssets()
{
	// Textures
	static ConstructorHelpers::FObjectFinder<UTexture> NotInteractableReticleObj(
		TEXT("/Game/Textures/NotInteractableReticleTexture")
	);
	NotInteractableReticleTexture = NotInteractableReticleObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> InteractableReticleObj(
		TEXT("/Game/Textures/InteractableReticleTexture")
	);
	InteractableReticleTexture = InteractableReticleObj.Object;
}

void ADefaultHUD::UpdateReticle()
{
	if (CurrentReticleTexture)
	{
		DrawTexture(CurrentReticleTexture, ViewportSize.X / 2, ViewportSize.Y / 2, 2.0f, 2.0f, 0, 0, 0, 0);
	}

	FCollisionQueryParams TraceParams(NAME_None, false, this);
	FHitResult HitResult;
	AActor* ActorHit = nullptr;

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerPtr->PlayerViewPointLocation,
		PlayerPtr->GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	ActorHit = HitResult.GetActor();
	if (!ActorHit)
	{
		GetWorld()->LineTraceSingleByObjectType(
			OUT HitResult,
			PlayerPtr->PlayerViewPointLocation,
			PlayerPtr->GetLineTraceEnd(),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel2),
			TraceParams
		);

		ActorHit = HitResult.GetActor();
	}
	if (InteractableReticleTexture && NotInteractableReticleTexture)
	{
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