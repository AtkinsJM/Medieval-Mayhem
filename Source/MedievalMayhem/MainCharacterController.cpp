// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "MainCharacterInputComponent.h"
#include "MainCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Enemy.h"

AMainCharacterController::AMainCharacterController()
{
	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;
	bMouseControlsCamera = false;
	bCharacterDirectionFixed = false;
	BaseZoomRate = 30.0f;

	InitialRotation = FRotator(-20.0f, 0.0f, 0.0f);

	bInvertYAxis = true;
}

void AMainCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	   
	if (!MainCharacterInputComponent)
	{
		MainCharacterInputComponent = NewObject<UMainCharacterInputComponent>(this);
		MainCharacterInputComponent->RegisterComponent();
		MainCharacterInputComponent->bBlockInput = false;
		MainCharacterInputComponent->Priority = 0;
		PushInputComponent(MainCharacterInputComponent);
	}

	check(MainCharacterInputComponent);

	MainCharacterInputComponent->BindAxis("ForwardMovement", this, &AMainCharacterController::MoveForward);
	MainCharacterInputComponent->BindAxis(TEXT("StrafeMovement"), this, &AMainCharacterController::Strafe);
	MainCharacterInputComponent->BindAxis(TEXT("MouseTurn"), this, &AMainCharacterController::TurnWithMouse);
	MainCharacterInputComponent->BindAxis(TEXT("Turn"), this, &AMainCharacterController::TurnWithKeyboard);
	MainCharacterInputComponent->BindAxis(TEXT("MouseLookUp"), this, &AMainCharacterController::LookUpWithMouse);
	MainCharacterInputComponent->BindAxis(TEXT("Zoom"), this, &AMainCharacterController::ZoomWithKeyboard);
	MainCharacterInputComponent->BindAxis(TEXT("MouseZoom"), this, &AMainCharacterController::ZoomWithMouse);

	MainCharacterInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Pressed, this, &AMainCharacterController::EnableCameraRotation);
	MainCharacterInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Released, this, &AMainCharacterController::DisableCameraRotation);
	MainCharacterInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Pressed, this, &AMainCharacterController::LockCharacterDirection);
	MainCharacterInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Released, this, &AMainCharacterController::UnlockCharacterDirection);
	MainCharacterInputComponent->BindAction(TEXT("BackwardMovement"), EInputEvent::IE_Pressed, this, &AMainCharacterController::StartBackwardMovement);
	MainCharacterInputComponent->BindAction(TEXT("BackwardMovement"), EInputEvent::IE_Released, this, &AMainCharacterController::EndBackwardMovement);
	
	MainCharacterInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainCharacterController::Jump);
	MainCharacterInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AMainCharacterController::StopJumping);
	
	MainCharacterInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Pressed, this, &AMainCharacterController::ToggleWalking);
	MainCharacterInputComponent->BindAction(TEXT("Walk"), EInputEvent::IE_Released, this, &AMainCharacterController::ToggleWalking);
	
	MainCharacterInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AMainCharacterController::PickUpItem);
	MainCharacterInputComponent->BindAction(TEXT("Drop"), EInputEvent::IE_Pressed, this, &AMainCharacterController::DropWeapon);
	MainCharacterInputComponent->BindAction(TEXT("SwapWeaponSet"), EInputEvent::IE_Pressed, this, &AMainCharacterController::SwapWeaponSet);

	MainCharacterInputComponent->BindAction(TEXT("SelectNextEnemy"), EInputEvent::IE_Pressed, this, &AMainCharacterController::SelectNextEnemy);
	
	//MainCharacterInputComponent->BindActionWithParam(FName("Weapon1"), EInputEvent::IE_Pressed, this, FName("EquipWeaponSet"), 0);
	//MainCharacterInputComponent->BindActionWithParam(FName("Weapon2"), EInputEvent::IE_Pressed, this, FName("EquipWeaponSet"), 1);

	MainCharacterInputComponent->BindActionWithParam(FName("WeaponSkill1"), EInputEvent::IE_Pressed, this, FName("UseWeaponSkill"), 1);
	MainCharacterInputComponent->BindActionWithParam(FName("WeaponSkill2"), EInputEvent::IE_Pressed, this, FName("UseWeaponSkill"), 2);
	MainCharacterInputComponent->BindActionWithParam(FName("WeaponSkill3"), EInputEvent::IE_Pressed, this, FName("UseWeaponSkill"), 3);
}

void AMainCharacterController::OnPossess(APawn * Pawn)
{
	Super::OnPossess(Pawn);

}

void AMainCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	MainCharacter = nullptr;
}

void AMainCharacterController::BeginPlay()
{
	Super::BeginPlay();

	ControlRotation += InitialRotation;
	
	MainCharacter = Cast<AMainCharacter>(GetPawn());
	
	if (MainCharacter)
	{
		CameraBoom = MainCharacter->CameraBoom;
	}
		
	if (HUDOverlayAsset != nullptr)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainCharacterController::MoveForward(float Value)
{	
	if (MainCharacter == nullptr || Value == 0.0f || MainCharacter->bIsAttacking) { return; }
	FRotator YawRotation(0.0f);
	if (!bMouseControlsCamera && !MainCharacter->bUseControllerRotationYaw)
	{
		ControlRotation.Yaw = MainCharacter->GetActorRotation().Yaw;
		MainCharacter->bUseControllerRotationYaw = true;
	}
	if (bMouseControlsCamera && !bCharacterDirectionFixed)
	{
		FRotator WantedRotation = MainCharacter->GetActorRotation();
		YawRotation.Yaw = WantedRotation.Yaw = ControlRotation.Yaw;
		MainCharacter->SetActorRotation(WantedRotation);
	}
	else
	{
		YawRotation.Yaw = MainCharacter->GetActorRotation().Yaw;
	}
	const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(YawRotation);
	MainCharacter->AddMovementInput(ForwardDirection, Value);
	
}
void AMainCharacterController::Strafe(float Value)
{
	if (MainCharacter == nullptr || Value == 0.0f || MainCharacter->bIsAttacking) { return; }
	FRotator YawRotation(0.0f);
	if (!bMouseControlsCamera && !MainCharacter->bUseControllerRotationYaw)
	{
		ControlRotation = (FRotator(ControlRotation.Pitch, MainCharacter->GetActorRotation().Yaw, ControlRotation.Roll));
		MainCharacter->bUseControllerRotationYaw = true;
	}
	if (bMouseControlsCamera && !bCharacterDirectionFixed)
	{
		FRotator WantedRotation = MainCharacter->GetActorRotation();
		YawRotation.Yaw = WantedRotation.Yaw = ControlRotation.Yaw;
		MainCharacter->SetActorRotation(WantedRotation);
	}
	else
	{
		YawRotation.Yaw = MainCharacter->GetActorRotation().Yaw;
	}
	const FVector RightDirection = UKismetMathLibrary::GetRightVector(YawRotation);
	MainCharacter->AddMovementInput(RightDirection, Value);
}

void  AMainCharacterController::TurnWithMouse(float Value)
{
	if (MainCharacter == nullptr || Value == 0.0f) { return; }
	if (bMouseControlsCamera)
	{
		ControlRotation.Yaw += Value;
	}
}

void AMainCharacterController::TurnWithKeyboard(float Value)
{
	if (MainCharacter == nullptr || Value == 0.0f || MainCharacter->bIsAttacking) { return; }
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

void AMainCharacterController::TurnAtRate(float Rate)
{
	if (bMouseControlsCamera)
	{
		MainCharacter->AddActorWorldRotation(FRotator(0.0f, Rate, 0.0f));
	}
	else
	{
		if (!MainCharacter->bUseControllerRotationYaw)
		{
			ControlRotation = (FRotator(ControlRotation.Pitch, MainCharacter->GetActorRotation().Yaw, ControlRotation.Roll));
			MainCharacter->bUseControllerRotationYaw = true;
		}
		// TODO Work out why I need to divide the rate by 2 for it to rotate correctly.
		// Is it getting called twice each frame somehow?
		ControlRotation.Yaw += (Rate / 2.0f);
	}
}

void  AMainCharacterController::LookUpWithMouse(float Value)
{
	if (MainCharacter == nullptr || Value == 0.0f) { return; }
	if (bMouseControlsCamera)
	{
		LookUpAtRate(Value);
	}
}

void  AMainCharacterController::LookUpWithKeyboard(float Value)
{
	// Calculate delta for this frame from the rate information  
	LookUpAtRate(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterController::LookUpAtRate(float Rate)
{
	if (bInvertYAxis) { Rate *= -1; }
	ControlRotation.Pitch += Rate;
	FRotator WantedRotation = ControlRotation;
	if (WantedRotation.Pitch > 180.0f) { WantedRotation.Pitch -= 360.0f; }
	WantedRotation.Pitch = FMath::Clamp(WantedRotation.Pitch, -60.0f, 0.0f);
	ControlRotation = WantedRotation;
}

void AMainCharacterController::ZoomWithKeyboard(float Value)
{
	if (CameraBoom == nullptr || Value == 0.0f) { return; }
	ZoomCameraAtRate(Value * BaseZoomRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterController::ZoomWithMouse(float Value)
{
	if (CameraBoom == nullptr || Value == 0.0f) { return; }
	ZoomCameraAtRate(Value * BaseZoomRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacterController::ZoomCameraAtRate(float Rate)
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - Rate, 300.0f, 1000.0f);
}

void AMainCharacterController::EnableCameraRotation()
{
	if (MainCharacter == nullptr) { return; }
	bMouseControlsCamera = true;
	MainCharacter->bUseControllerRotationYaw = false;
}

void AMainCharacterController::DisableCameraRotation()
{
	bMouseControlsCamera = false;
}


void AMainCharacterController::LockCharacterDirection()
{
	bCharacterDirectionFixed = true;
}

void AMainCharacterController::UnlockCharacterDirection()
{
	bCharacterDirectionFixed = false;

}

void AMainCharacterController::ToggleWalking()
{
	if (MainCharacter == nullptr) { return; }
	bIsWalking = !bIsWalking;
	if (bIsWalking)
	{
		MainCharacter->SetMovementStatus(EMovementStatus::EMS_Walking);
	}
	else
	{
		MainCharacter->SetMovementStatus(EMovementStatus::EMS_Normal);
	}
}

void AMainCharacterController::Jump()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->Jump();
}

void AMainCharacterController::StopJumping()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->StopJumping();
}

void AMainCharacterController::PickUpItem()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->PickUpItem();
}

void AMainCharacterController::DropWeapon()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->DropWeapon();
}

void AMainCharacterController::SwapWeaponSet()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->SwapWeaponSet();
}

void AMainCharacterController::SelectNextEnemy()
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->SelectNextEnemy();
}

void AMainCharacterController::EquipWeaponSet(int32 Index)
{
	if (MainCharacter == nullptr) { return; }
	MainCharacter->EquipWeaponSet(Index);
}

void AMainCharacterController::UseWeaponSkill(int32 Index)
{
	if (MainCharacter == nullptr) { return; }
	AEnemy* Target = MainCharacter->GetAttackTarget();
	if (Target)
	{
		MainCharacter->bUseControllerRotationYaw = false;
		FRotator WantedRotation = MainCharacter->GetActorRotation();
		WantedRotation.Yaw = GetLookAtRotation(Target->GetActorLocation()).Yaw;
		MainCharacter->SetActorRotation(WantedRotation);
	}
	MainCharacter->UseWeaponSkill(Index);
}

FRotator AMainCharacterController::GetLookAtRotation(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MainCharacter->GetActorLocation(), Target);
	return LookAtRotation;
}

void AMainCharacterController::StartBackwardMovement()
{
	if (MainCharacter == nullptr) { return; }
	bIsMovingBackwards = true;
	MainCharacter->SetMovementStatus(EMovementStatus::EMS_Walking);
}

void AMainCharacterController::EndBackwardMovement()
{
	if (MainCharacter == nullptr) { return; }
	bIsMovingBackwards = false;
	if (!bIsWalking)
	{
		MainCharacter->SetMovementStatus(EMovementStatus::EMS_Normal);
	}
}
