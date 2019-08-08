// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CollidingPawn.generated.h"

UCLASS()
class MEDIEVALMAYHEM_API ACollidingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollidingPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCollidingPawnMovementComponent* MovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float CameraZoomSpeed;

	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Value) { MeshComponent = Value; }

	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Value) { SphereComponent = Value; }

	FORCEINLINE UCameraComponent* GetCamera() { return Camera; }
	FORCEINLINE void SetCamera(UCameraComponent* Value) { Camera = Value; }

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* Value) { SpringArm = Value; }

private:
	void MoveY(float Value);
	void MoveX(float Value);
	void PitchCamera(float Value);
	void YawCamera(float Value);
	void ZoomCamera(float Value);
	void EnableCameraRotation();
	void DisableCameraRotation();
	void RotateCamera();

	FVector2D CameraInput;
	bool bCanRotateCamera = false;
};
