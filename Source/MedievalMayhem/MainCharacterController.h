// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALMAYHEM_API AMainCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainCharacterController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;
		
	class UMainCharacterInputComponent* MainCharacterInputComponent;

	class AMainCharacter* MainCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	FRotator InitialRotation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float AirControl;
		
	/** Base rotation rates to scale rotation functions for the camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseZoomRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	bool bInvertYAxis;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* Pawn) override;

	virtual void OnUnPossess() override;

	

public:
	void MoveForward(float Value);
	void Strafe(float Value);

	void TurnWithMouse(float Value);
	void TurnWithKeyboard(float Value);
	void TurnAtRate(float Rate);

	void LookUpWithMouse(float Value);
	void LookUpWithKeyboard(float Value);
	void LookUpAtRate(float Rate);

	void ZoomWithKeyboard(float Value);
	void ZoomWithMouse(float Value);
	void ZoomCameraAtRate(float Rate);

	void EnableCameraRotation();

	void DisableCameraRotation();


	void LockCharacterDirection();
	void UnlockCharacterDirection();

	void StartBackwardMovement();
	void EndBackwardMovement();

	void ToggleWalking();

	void Jump();
	void StopJumping();

	void PickUpWeapon();
	void DropWeapon();
	void SwapWeaponSet();

	void SelectNextEnemy();

	void ConsumeHealthPotion();
	void ConsumeStaminaPotion();

	UFUNCTION()
	void EquipWeaponSet(int32 Index);

	UFUNCTION()
	void UseWeaponSkill(int32 Index);

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	FRotator GetLookAtRotation(FVector Target);

	FORCEINLINE AMainCharacter* GetMainCharacter() { return MainCharacter; }

	void TogglePause();


private:
	class USpringArmComponent* CameraBoom;

	class UMedievalMayhemGameInstance* GameInstance;

	bool bMouseControlsCamera;
	bool bCharacterDirectionFixed;
	bool bIsWalking;
	bool bIsMovingBackwards;
};
