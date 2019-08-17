// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	// Attach camera to end of boom and let boom control its rotation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Set size for capsule
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;
	bMouseControlsCamera = false;
	bCharacterDirectionFixed = false;
	BaseZoomRate = 30.0f;
	BackwardSpeed = 225.0f;

	InitialRotation = FRotator(-20.0f, 0.0f, 0.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	MaxHealth = 100.0f;
	Health = 100.0f;
	MaxStamina = 100.0f;
	Stamina = 100.0f;
	Coins = 0;

	RunningSpeed = 450.0f;
	WalkingSpeed = 200.0f;

	bIsWalking = false;
	bIsMovingBackwards = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	Controller->SetControlRotation(GetControlRotation() + InitialRotation);
	Health = MaxHealth;
	Stamina = MaxStamina;

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AMainCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			Die();
		}
	}
	return ActualDamage;
}

void AMainCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died!"));
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("ForwardMovement"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("StrafeMovement"), this, &AMainCharacter::Strafe);
	PlayerInputComponent->BindAxis(TEXT("MouseTurn"), this, &AMainCharacter::TurnWithMouse);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMainCharacter::TurnWithKeyboard);
	PlayerInputComponent->BindAxis(TEXT("MouseLookUp"), this, &AMainCharacter::LookUpWithMouse);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &AMainCharacter::ZoomWithKeyboard);
	PlayerInputComponent->BindAxis(TEXT("MouseZoom"), this, &AMainCharacter::ZoomWithMouse);

	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Pressed, this, &AMainCharacter::EnableCameraRotation);
	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Released, this, &AMainCharacter::DisableCameraRotation);
	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Pressed, this, &AMainCharacter::LockCharacterDirection);
	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Released, this, &AMainCharacter::UnlockCharacterDirection);
	PlayerInputComponent->BindAction(TEXT("BackwardMovement"), EInputEvent::IE_Pressed, this, &AMainCharacter::StartBackwardMovement);
	PlayerInputComponent->BindAction(TEXT("BackwardMovement"), EInputEvent::IE_Released, this, &AMainCharacter::EndBackwardMovement);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Pressed, this, &AMainCharacter::ToggleWalking);
	PlayerInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Released, this, &AMainCharacter::ToggleWalking);
}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller == nullptr || Value == 0.0f) { return; }
	FRotator YawRotation(0.0f);
	if (!bMouseControlsCamera && !bUseControllerRotationYaw)
	{
		Controller->SetControlRotation(FRotator(GetControlRotation().Pitch, GetActorRotation().Yaw, GetControlRotation().Roll));
		bUseControllerRotationYaw = true;
	}
	if (bMouseControlsCamera && !bCharacterDirectionFixed)
	{
		FRotator WantedRotation = GetActorRotation();
		YawRotation.Yaw = WantedRotation.Yaw = Controller->GetControlRotation().Yaw;
		SetActorRotation(WantedRotation);
	}
	else
	{
		YawRotation.Yaw = GetActorRotation().Yaw;
	}
	const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(YawRotation);
	AddMovementInput(ForwardDirection, Value);
}

void AMainCharacter::Strafe(float Value)
{
	if (Controller == nullptr || Value == 0.0f) { return; }
	FRotator YawRotation(0.0f);
	if (!bMouseControlsCamera && !bUseControllerRotationYaw)
	{
		Controller->SetControlRotation(FRotator(GetControlRotation().Pitch, GetActorRotation().Yaw, GetControlRotation().Roll));
		bUseControllerRotationYaw = true;
	}
	if (bMouseControlsCamera && !bCharacterDirectionFixed)
	{
		FRotator WantedRotation = GetActorRotation();
		YawRotation.Yaw = WantedRotation.Yaw = Controller->GetControlRotation().Yaw;
		SetActorRotation(WantedRotation);
	}
	else
	{
		YawRotation.Yaw = GetActorRotation().Yaw;
	}
	const FVector RightDirection = UKismetMathLibrary::GetRightVector(YawRotation);
	AddMovementInput(RightDirection, Value);
}

void  AMainCharacter::TurnWithMouse(float Value)
{
	if (Controller == nullptr || Value == 0.0f) { return; }
	if (bMouseControlsCamera)
	{
		AddControllerYawInput(Value);
	}
}

void AMainCharacter::TurnWithKeyboard(float Value)
{
	if (Controller == nullptr || Value == 0.0f) { return; }
	// Calculate delta for this frame from the rate information  
	if (bMouseControlsCamera && !bCharacterDirectionFixed)
	{
		Strafe(Value);
	}
	else
	{
		TurnAtRate(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}	
}

void AMainCharacter::TurnAtRate(float Rate)
{
	if (bMouseControlsCamera)
	{
		AddActorLocalRotation(FRotator(0.0f, Rate, 0.0f));	
	}
	else
	{
		if (bUseControllerRotationYaw == false)
		{
			Controller->SetControlRotation(FRotator(GetControlRotation().Pitch, GetActorRotation().Yaw, GetControlRotation().Roll));
			bUseControllerRotationYaw = true;
		}
		AddControllerYawInput(Rate);
	}
}

void  AMainCharacter::LookUpWithMouse(float Value)
{
	if (Controller == nullptr || Value == 0.0f) { return; }
	if (bMouseControlsCamera)
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
	FRotator WantedRotation = Controller->GetControlRotation();
	if (WantedRotation.Pitch > 180.0f) { WantedRotation.Pitch -= 360.0f; }
	WantedRotation.Pitch = FMath::Clamp(WantedRotation.Pitch, -60.0f, 0.0f);
	Controller->SetControlRotation(WantedRotation);
}

void AMainCharacter::ZoomWithKeyboard(float Value)
{
	if (CameraBoom == nullptr || Value == 0.0f) { return; }
	ZoomCameraAtRate(Value * BaseZoomRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::ZoomWithMouse(float Value)
{
	if (CameraBoom == nullptr || Value == 0.0f) { return; }
	ZoomCameraAtRate(Value * BaseZoomRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::ZoomCameraAtRate(float Rate)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - Rate, 300.0f, 1000.0f);
}

void AMainCharacter::EnableCameraRotation()
{
	bMouseControlsCamera = true;
	bUseControllerRotationYaw = false;
}

void AMainCharacter::DisableCameraRotation()
{
	bMouseControlsCamera = false;
}


void AMainCharacter::LockCharacterDirection()
{
	bCharacterDirectionFixed = true;
}

void AMainCharacter::UnlockCharacterDirection()
{
	bCharacterDirectionFixed = false;

}

void AMainCharacter::StartBackwardMovement()
{
	bIsMovingBackwards = true;
	SetMovementStatus(EMovementStatus::EMS_Walking);
}

void AMainCharacter::EndBackwardMovement()
{
	bIsMovingBackwards = false;
	if (!bIsWalking)
	{
		SetMovementStatus(EMovementStatus::EMS_Normal);
	}
}

void AMainCharacter::PickupCoin(FVector Location, int32 Amount)
{
	PickupLocations.Add(Location);

	UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 12, FLinearColor::Green, 99.9f, 0.5f);
	IncrementCoins(Amount);
}

void AMainCharacter::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	switch (MovementStatus)
	{
		case EMovementStatus::EMS_Walking:
			GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
			break;
		case EMovementStatus::EMS_Normal:
			GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
			break;
		default:
			GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::ToggleWalking()
{
	bIsWalking = !bIsWalking;
	if (bIsWalking)
	{
		SetMovementStatus(EMovementStatus::EMS_Walking);
	}
	else
	{
		SetMovementStatus(EMovementStatus::EMS_Normal);	
	}
}