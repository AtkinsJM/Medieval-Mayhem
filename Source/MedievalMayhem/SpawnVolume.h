// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn Volume")
	class UBoxComponent* SpawningBox;

	UPROPERTY(EditANywhere, BlueprintReadOnly, Category = "Spawn Volume")
	TSubclassOf<APawn> PawnToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Spawn Volume")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawn Volume")
	void SpawnPawn(const FVector SpawnLocation);

private:
	FVector BoxExtent;
	FVector BoxOrigin;
};
