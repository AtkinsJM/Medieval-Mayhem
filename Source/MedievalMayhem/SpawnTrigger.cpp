// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnTrigger.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "MainCharacter.h"

// Sets default values
ASpawnTrigger::ASpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnTriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Spawn Trigger Sphere"));
	SpawnTriggerSphere->SetupAttachment(GetRootComponent());


	SpawnTriggerRadius = 800.0f;

	bSpawned = false;
}

// Called when the game starts or when spawned
void ASpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTriggerSphere->SetSphereRadius(SpawnTriggerRadius);

	SpawnTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASpawnTrigger::OnSpawnTriggerSphereBeginOverlap);
}

// Called every frame
void ASpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnTrigger::OnSpawnTriggerSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && !bSpawned)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning triggered!"));
			SpawnPawn();
		}
	}
}

void ASpawnTrigger::SpawnPawn()
{
	if (PawnToSpawn)
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<APawn>(PawnToSpawn, GetActorLocation(), FRotator(0.0f), SpawnParams);
		bSpawned = true;
		Destroy();
	}
}

