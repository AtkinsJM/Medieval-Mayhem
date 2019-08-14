// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(Root);
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	PlatformMesh->SetupAttachment(GetRootComponent());

	TriggerEnterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Enter Mesh"));
	TriggerEnterMesh->SetupAttachment(GetRootComponent());
	TriggerEnterMesh->SetHiddenInGame(true);
	TriggerEnterMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	
	TriggerExitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Exit Mesh"));
	TriggerExitMesh->SetupAttachment(GetRootComponent());
	TriggerExitMesh->SetHiddenInGame(true);
	TriggerExitMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	bFloats = true;
	Frequency = 1.0f;
	Amplitude = 100.0f;
	DropDistance = 40.0f;

	Offset = 0.0f;

	DropSpeed = 2.0f;
	bIsOccupied = false;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerEnterMesh->OnComponentBeginOverlap.AddDynamic(this, &AFloatingPlatform::OnBeginOverlap);
	TriggerExitMesh->OnComponentEndOverlap.AddDynamic(this, &AFloatingPlatform::OnEndOverlap);

	Origin = InitialLocation = GetActorLocation();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bFloats)
	{
		Float();

		if (bIsOccupied)
		{
			Drop(DeltaTime);
		}
		else
		{
			Rise(DeltaTime);
		}
	}
}

void AFloatingPlatform::Float()
{
	FVector Location = Origin + FVector(0.0f, 0.0f, FMath::Sin(GetWorld()->GetTimeSeconds() * Frequency + (Offset * PI/2)) * (Amplitude/2.0f));
	SetActorLocation(Location);
}

void AFloatingPlatform::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bIsOccupied = true;
	
	//InitialLocation.Z -= DropDistance;
}

void AFloatingPlatform::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsOccupied = false;
	//InitialLocation.Z += DropDistance;
}

void AFloatingPlatform::Drop(float DeltaTime)
{
	Origin.Z = FMath::FInterpTo(Origin.Z, InitialLocation.Z - DropDistance, DeltaTime, DropSpeed);
}

void AFloatingPlatform::Rise(float DeltaTime)
{
	Origin.Z = FMath::FInterpTo(Origin.Z, InitialLocation.Z, DeltaTime, DropSpeed);
}