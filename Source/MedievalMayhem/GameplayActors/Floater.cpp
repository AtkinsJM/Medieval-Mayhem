// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));

	InitialLocation = FVector(0.0f, 0.0f, 0.0f);
	PlacedLocation = FVector(0.0f, 0.0f, 0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	InitialDirection = FVector(0.0f, 0.0f, 0.0f);
	bShouldFloat = false;
	bInitialiseFloaterLocation = false;
	InitialForce = FVector(2000000.0f, 0.0f, 0.0f);
	InitialTorque = FVector(0.0f, 0.0f, 0.0f);
	Amplitude = 100.0f;

	RunningTime = 0.0f;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();
	
	float InitialX = FMath::FRandRange(-100.0f, 100.0f);
	float InitialY = FMath::FRandRange(-100.0f, 100.0f);
	float InitialZ = FMath::FRandRange(-100.0f, 100.0f);

	InitialLocation = FVector(InitialX, InitialY, InitialZ);

	PlacedLocation = GetActorLocation();
	if (bInitialiseFloaterLocation)
	{
		SetActorLocation(InitialLocation);
	}
	
	//StaticMesh->AddForce(InitialForce);
	//StaticMesh->AddTorque(InitialTorque);
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bShouldFloat)
	{
		RunningTime += DeltaTime;

		MoveInCircle();

	}
}

void AFloater::MoveInCircle()
{
	FVector NewLocation = PlacedLocation + FVector(FMath::Sin(RunningTime) * Amplitude, FMath::Cos(RunningTime) * Amplitude, 0.0f);
	SetActorLocation(NewLocation);
}

