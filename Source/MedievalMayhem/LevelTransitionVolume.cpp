// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"
#include "Kismet/GameplayStatics.h"
#include "MainCharacter.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TransitionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Transition Box"));
}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();
	TransitionBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnBeginOverlap);
}

void ALevelTransitionVolume::OnBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (MainCharacter)
	{
		MainCharacter->SaveGame(true, "TransitionSave");
		LoadLevel();
	}
}

void ALevelTransitionVolume::LoadLevel()
{
	if (LevelToLoad != "")
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
	}
}


