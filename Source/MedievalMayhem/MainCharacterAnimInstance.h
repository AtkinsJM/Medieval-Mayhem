// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALMAYHEM_API UMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category="Animation Properties")
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float ForwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StrafeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	class APawn* Pawn;
};
