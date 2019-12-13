// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MedievalMayhemSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Data")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FRotator Rotation;
};

/**
 * 
 */
UCLASS()
class MEDIEVALMAYHEM_API UMedievalMayhemSaveGame : public USaveGame
{
	GENERATED_BODY()

	public:
		UMedievalMayhemSaveGame();

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		FName PlayerName;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		uint32 UserIndex;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		FCharacterStats CharacterStats;
	
};
