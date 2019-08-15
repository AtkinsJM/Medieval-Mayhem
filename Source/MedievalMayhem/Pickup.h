// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALMAYHEM_API APickup : public AInteractableItem
{
	GENERATED_BODY()
	APickup();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	int32 Value;

public:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
