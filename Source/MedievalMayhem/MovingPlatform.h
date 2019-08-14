// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
		class UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		bool bMoves;

	UPROPERTY(EditAnywhere, Category = "Platform")
		FVector StartLocation;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Platform")
		FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		float InterpDelay;

	FTimerHandle InterpTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
		bool bInterpolating;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MovePlatform(float DeltaTime);

	void ToggleInterpolating();

	void SwapVectors(FVector& Vector1, FVector& Vector2);

private:
	FVector InitialLocation;
};
