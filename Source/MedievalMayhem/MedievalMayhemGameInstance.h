// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MedievalMayhemGameInstance.generated.h"

/**
 * 
 */

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
		int32 HealthPotions;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		int32 StaminaPotions;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		FString Weapon1;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		FString Weapon2;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		int32 CurrentWeaponSet;
};

USTRUCT(BlueprintType)
struct FWorldData
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere, Category = "Data")
		FName MapName;

};

UCLASS()
class MEDIEVALMAYHEM_API UMedievalMayhemGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMedievalMayhemGameInstance();

	void SaveGame(FString SlotName);
	void LoadGame(FString SlotName);

	void FinishSaveLoad();

	void LoadLevel(FString LevelName);

	FCharacterStats CharacterStats;
	FWorldData WorldData;

	bool bIsNewLevel;

	bool bIsNewGame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool bIsTransitioning;

	void FinishTransition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool bIsSaving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool bIsLoading;
	   
	FTimerHandle SaveLoadTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FString CurrentLevel;
};
