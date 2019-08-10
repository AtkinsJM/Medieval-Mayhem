// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		float MovementSpeed = LateralSpeed.Size();
		float YawRotation = Pawn->GetActorRotation().Yaw;
		//UE_LOG(LogTemp, Warning, TEXT("Yaw rotation: %f"), YawRotation);
		//ForwardSpeed = MovementSpeed * FMath::Cos(YawRotation);
		//StrafeSpeed = MovementSpeed * FMath::Sin(YawRotation);
		
		ForwardSpeed = FVector::DotProduct(Speed, Pawn->GetActorForwardVector());
		StrafeSpeed = FVector::DotProduct(Speed, Pawn->GetActorRightVector());
		UE_LOG(LogTemp, Warning, TEXT("Forward speed: %f"), ForwardSpeed);
		UE_LOG(LogTemp, Warning, TEXT("Strafe speed: %f"), StrafeSpeed);
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}