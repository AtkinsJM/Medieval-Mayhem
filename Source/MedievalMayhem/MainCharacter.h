// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Walking UMETA(DisplayName = "Walking"),

	EMS_MAX UMETA(DisplayName = "Max")
};

UCLASS()
class MEDIEVALMAYHEM_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementStatus MovementStatus;

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category="Camera")
	float DefaultCameraBoomLength = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	FRotator InitialRotation;

	void SetMovementStatus(EMovementStatus Status);

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

	void Die();
	void IncrementCoins(int32 Amount);

	void ToggleWalking();

	/** Base rotation rates to scale rotation functions for the camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseZoomRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BackwardSpeed;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** 
	* PLAYER STATS 
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkingSpeed;

private:
	bool bMouseControlsCamera;
	bool bCharacterDirectionFixed;
	bool bIsWalking;
	bool bIsMovingBackwards;
};
