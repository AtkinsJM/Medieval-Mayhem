// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "MainCharacter.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	if (Pawn && !MainCharacter)
	{
		MainCharacter = Cast<AMainCharacter>(Pawn);
	}
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn)
	{
		if (LastYaw == 0.0f) { LastYaw = Pawn->GetActorRotation().Yaw; }
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		ForwardSpeed = FVector::DotProduct(Speed, Pawn->GetActorForwardVector());
		StrafeSpeed = FVector::DotProduct(Speed, Pawn->GetActorRightVector());

		YawDelta = Pawn->GetActorRotation().Yaw - LastYaw;
		if (YawDelta > 180.0f) { YawDelta -= 360.0f; }
		if (YawDelta < -180.0f) { YawDelta += 360.0f; }
		LastYaw = Pawn->GetActorRotation().Yaw;

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		bIsAlive = MainCharacter->GetIsAlive();
	}
}