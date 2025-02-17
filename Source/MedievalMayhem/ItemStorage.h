// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

class AWeapon;

UCLASS()
class MEDIEVALMAYHEM_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

	FORCEINLINE TSubclassOf<AWeapon> GetWeapon(FString WeaponId) { return WeaponMap[WeaponId]; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TMap<FString, TSubclassOf<AWeapon>> WeaponMap;
};
