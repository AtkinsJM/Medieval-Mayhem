// Fill out your copyright notice in the Description page of Project Settings.


#include "MedievalMayhemGameInstance.h"
#include "MedievalMayhemSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UMedievalMayhemGameInstance::UMedievalMayhemGameInstance()
{
	bIsNewLevel = false;
	bIsNewGame = true;

	bIsSaving = false;
	bIsLoading = false;
}

void UMedievalMayhemGameInstance::SaveGame(FString SlotName)
{
	bIsSaving = true;

	UMedievalMayhemSaveGame* SaveGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats = CharacterStats;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	WorldData.MapName = FName(*MapName);
	SaveGameInstance->WorldData = WorldData;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName != "" ? SlotName : SaveGameInstance->SlotName, SaveGameInstance->UserIndex);

	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &UMedievalMayhemGameInstance::FinishSaveLoad, 1.0f, true);
}

void UMedievalMayhemGameInstance::LoadGame(FString SlotName)
{
	UMedievalMayhemSaveGame* LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));
	LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName != "" ? SlotName : LoadGameInstance->SlotName, LoadGameInstance->UserIndex));

	// If saved game with specified slot name can't be found, don't try loading
	if (!LoadGameInstance) { return; }

	bIsLoading = true;
	bIsNewLevel = false;

	CharacterStats = LoadGameInstance->CharacterStats;
	WorldData = LoadGameInstance->WorldData;

	// Check for initial game startup load to ensure map transition takes place if needed
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (LoadGameInstance->WorldData.MapName != TEXT(""))
	{
		FName LevelToLoad = LoadGameInstance->WorldData.MapName;
		UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
	}

	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &UMedievalMayhemGameInstance::FinishSaveLoad, 1.0f, true);
}

void UMedievalMayhemGameInstance::FinishSaveLoad()
{
	bIsSaving = bIsLoading = false;
}