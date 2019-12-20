// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MedievalMayhemGameInstance.h"
#include "MedievalMayhemSaveGame.generated.h"


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
		FString SlotName;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		uint32 UserIndex;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		FCharacterStats CharacterStats;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		FWorldData WorldData;

		UPROPERTY(VisibleAnywhere, Category = "Basic")
		bool bIsTransitionSave;
	
};
