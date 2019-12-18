// Fill out your copyright notice in the Description page of Project Settings.


#include "MedievalMayhemSaveGame.h"

UMedievalMayhemSaveGame::UMedievalMayhemSaveGame()
{
	SlotName = TEXT("Default");
	UserIndex = 0;
	WorldData.MapName = TEXT("");
	bIsTransitionSave = false;
}