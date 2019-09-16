// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "MainCharacter.h"

// Sets default values
AInteractableItem::AInteractableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Volume"));
	RootComponent = CollisionVolume;

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractableItem::OnBeginOverlap);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractableItem::OnEndOverlap);
	
	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Idle Particles Component"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	bRotates = false;
	RotationRate = 10.0f;

	bFloats = false;
	Frequency = 1.0f;
	Amplitude = 100.0f;

	bInteractsByOverlap = false;
}

// Called when the game starts or when spawned
void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	
	Origin = GetActorLocation();
}

// Called every frame
void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotates)
	{
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += RotationRate * DeltaTime;
		SetActorRotation(Rotation);
	}
	if (bFloats)
	{
		Float();
	}
}

void AInteractableItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!bInteractsByOverlap || !Cast<AMainCharacter>(OtherActor)) { return; }

	if (OverlapParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.0f), true);
	}
	if (OverlapSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, OverlapSound);
	}
	IdleParticlesComponent->Deactivate();
}

void AInteractableItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AInteractableItem::Float()
{
	FVector Location = Origin + FVector(0.0f, 0.0f, FMath::Sin(GetWorld()->GetTimeSeconds() * Frequency) * (Amplitude / 2.0f));
	SetActorLocation(Location);
}