// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(Root);
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	PlatformMesh->SetupAttachment(GetRootComponent());

	StartLocation = FVector(0.0f);
	EndLocation = FVector(0.0f);

	InterpSpeed = 1.0f;
	InterpDelay = 1.0f;

	bInterpolating = false;
	bMoves = false;
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();

	if (bMoves)
	{
		StartLocation = InitialLocation;

		EndLocation += StartLocation;

		Distance = (EndLocation - StartLocation).Size();

		GetWorldTimerManager().SetTimer(InterpTimer, this, &AMovingPlatform::ToggleInterpolating, InterpDelay);
	}
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoves && bInterpolating)
	{
		MovePlatform(DeltaTime);
		float DistanceTravelled = (GetActorLocation() - StartLocation).Size();
		if (Distance - DistanceTravelled < 2.0f)
		{
			ToggleInterpolating();
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AMovingPlatform::ToggleInterpolating, InterpDelay);
			SwapVectors(StartLocation, EndLocation);
		}
	}
}

void AMovingPlatform::MovePlatform(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector Interp = FMath::VInterpTo(CurrentLocation, EndLocation, DeltaTime, InterpSpeed);

	SetActorLocation(Interp);
}

void AMovingPlatform::ToggleInterpolating()
{
	bInterpolating = !bInterpolating;
}

void AMovingPlatform::SwapVectors(FVector& Vector1, FVector& Vector2)
{
	FVector Temp = Vector1;
	Vector1 = Vector2;
	Vector2 = Temp;
}