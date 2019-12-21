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
	bIsTransitioning = false;
	
	bIsPaused = false;
}

void UMedievalMayhemGameInstance::SaveGame(FString SlotName)
{
	SetPauseState(false);
	
	UMedievalMayhemSaveGame* SaveGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));

	if (!SaveGameInstance) { return; }
	
	bIsSaving = true;

	SaveGameInstance->CharacterStats = CharacterStats;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	WorldData.MapName = MapName;
	SaveGameInstance->WorldData = WorldData;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName != "" ? SlotName : SaveGameInstance->SlotName, SaveGameInstance->UserIndex);

	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &UMedievalMayhemGameInstance::FinishSaveLoad, 1.0f, true);
}

void UMedievalMayhemGameInstance::LoadGame(FString SlotName)
{
	SetPauseState(false);

	UMedievalMayhemSaveGame* LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));
	LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName != "" ? SlotName : LoadGameInstance->SlotName, LoadGameInstance->UserIndex));

	// If saved game with specified slot name can't be found, don't try loading
	if (!LoadGameInstance) { return; }

	bIsLoading = true;
	bIsNewLevel = false;

	UE_LOG(LogTemp, Warning, TEXT("Loading!"));
	CharacterStats = LoadGameInstance->CharacterStats;
	WorldData = LoadGameInstance->WorldData;

	FString MapName = LoadGameInstance->WorldData.MapName;
	if (MapName != "")
	{
		LoadLevel(MapName);
	}	

	//TODO finish load on finish transition (i.e., in MainPlayer for now...)?
	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &UMedievalMayhemGameInstance::FinishSaveLoad, 1.0f, true);
}

void UMedievalMayhemGameInstance::FinishSaveLoad()
{
	bIsSaving = bIsLoading = false;
}

void UMedievalMayhemGameInstance::LoadLevel(FString LevelName)
{
	if (LevelName == "") { return; }
	CurrentLevel = LevelName;
	bIsTransitioning = true;
	UE_LOG(LogTemp, Warning, TEXT("Transitioning!"));
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

//TODO: move pausing across to game instance class(?)
//TODO: unpause game automatically on save/load, rather than as part of button functionality - see if that fixes all my problems!
void UMedievalMayhemGameInstance::SetPauseState(bool val)
{
	bIsPaused = val;
	if (UGameplayStatics::IsGamePaused(GetWorld()) != bIsPaused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), bIsPaused);
	}
}