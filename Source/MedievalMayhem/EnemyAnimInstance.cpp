// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Enemy.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (!Pawn)
	{
		Pawn = TryGetPawnOwner();
	}
	if (Pawn && !Enemy)
	{
		Enemy = Cast<AEnemy>(Pawn);
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		ForwardSpeed = FVector::DotProduct(Speed, Pawn->GetActorForwardVector());
		StrafeSpeed = FVector::DotProduct(Speed, Pawn->GetActorRightVector());
	}
}