// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawning Box"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	BoxExtent = SpawningBox->GetScaledBoxExtent();
	BoxOrigin = SpawningBox->GetComponentLocation();
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(BoxOrigin, BoxExtent);
	return Point;
}

void ASpawnVolume::SpawnAtLocation_Implementation(const FVector SpawnLocation)
{
	if (SpawnArray.Num() > 0)
	{
		TSubclassOf<AActor> ActorToSpawn = SpawnArray[FMath::RandRange(0, SpawnArray.Num() - 1)];
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnLocation, FRotator(0.0f), SpawnParams);
	}
}
