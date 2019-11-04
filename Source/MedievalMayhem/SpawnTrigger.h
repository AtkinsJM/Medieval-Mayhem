// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnTrigger.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API ASpawnTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnTrigger();

	/* Volume within which the spawning is triggered*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class USphereComponent* SpawnTriggerSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	float SpawnTriggerRadius;

	UPROPERTY(EditANywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<APawn> PawnToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnSpawnTriggerSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void SpawnPawn();

private:
	bool bSpawned;
};
