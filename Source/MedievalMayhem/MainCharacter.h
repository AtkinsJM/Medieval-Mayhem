// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category="Camera")
	float DefaultCameraBoomLength = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	FRotator InitialRotation;

	void MoveForward(float Value);

	void Strafe(float Value);

	void TurnWithMouse(float Value);
	void TurnWithKeyboard(float Value);
	void TurnAtRate(float Rate);

	void LookUpWithMouse(float Value);
	void LookUpWithKeyboard(float Value);
	void LookUpAtRate(float Rate);

	void ZoomCamera(float Value);

	void EnableCameraRotation();

	void DisableCameraRotation();


	void LockCharacterDirection();
	void UnlockCharacterDirection();

	/** Base rotation rates to scale rotation functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraZoomSpeed;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	bool bMouseControlsCamera;
	bool bCharacterDirectionFixed;
};
