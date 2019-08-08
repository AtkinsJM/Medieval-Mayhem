// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = DefaultCameraBoomLength;
	CameraBoom->bUsePawnControlRotation = true;

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	// Attach camera to end of boom and let boom control its rotation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Set size for capsule
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;
	bCanRotateCamera = false;
	CameraZoomSpeed = 10.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MouseTurn"), this, &AMainCharacter::TurnWithMouse);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMainCharacter::TurnWithKeyboard);
	PlayerInputComponent->BindAxis(TEXT("MouseLookUp"), this, &AMainCharacter::LookUpWithMouse);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("CameraZoom"), this, &AMainCharacter::ZoomCamera);

	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Pressed, this, &AMainCharacter::EnableCameraRotation);
	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Released, this, &AMainCharacter::DisableCameraRotation);
	PlayerInputComponent->BindAction(TEXT("RotateCharacter"), EInputEvent::IE_Pressed, this, &AMainCharacter::EnableCharacterRotation);
	PlayerInputComponent->BindAction(TEXT("RotateCharacter"), EInputEvent::IE_Released, this, &AMainCharacter::DisableCharacterRotation);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);

}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		//const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FRotator YawRotation(0.0f, GetActorRotation().Yaw, 0.0f);

		const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(YawRotation);
		AddMovementInput(ForwardDirection, Value);
	}	
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		//const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FRotator YawRotation(0.0f, GetActorRotation().Yaw, 0.0f);

		const FVector RightDirection = UKismetMathLibrary::GetRightVector(YawRotation);
		AddMovementInput(RightDirection, Value);
	}
}

void  AMainCharacter::TurnWithMouse(float Value)
{
	if (bCanRotateCamera)
	{
		TurnAtRate(Value);
	}
}

void AMainCharacter::TurnWithKeyboard(float Value)
{
	// Calculate delta for this frame from the rate information  
	TurnAtRate(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate);
}

void  AMainCharacter::LookUpWithMouse(float Value)
{
	if (bCanRotateCamera)
	{
		LookUpAtRate(Value);
	}
}

void  AMainCharacter::LookUpWithKeyboard(float Value)
{
	// Calculate delta for this frame from the rate information  
	LookUpAtRate(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate);
}

void AMainCharacter::ZoomCamera(float Value)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - (Value * CameraZoomSpeed), 200.0f, 600.0f);
}

void AMainCharacter::EnableCameraRotation()
{
	bCanRotateCamera = true;
}

void AMainCharacter::DisableCameraRotation()
{
	bCanRotateCamera = false;
}

void AMainCharacter::EnableCharacterRotation()
{
	bUseControllerRotationYaw = true;
}

void AMainCharacter::DisableCharacterRotation()
{
	bUseControllerRotationYaw = false;
}