// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* TriggerEnterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* TriggerExitMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bFloats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float Frequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float Offset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float DropDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float DropSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Float();

	void Drop(float DeltaTime);
	void Rise(float DeltaTime);

private:
	FVector InitialLocation;

	FVector Origin;

	bool bIsOccupied;
};
