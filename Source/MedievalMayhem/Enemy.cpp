// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Classes/Animation/AnimInstance.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StartFollowSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Start Follow Sphere"));
	StartFollowSphere->SetupAttachment(GetRootComponent());
	StartFollowSphere->SetVisibility(true);
		
	StopFollowSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Stop Follow Sphere"));
	StopFollowSphere->SetupAttachment(GetRootComponent());

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(GetRootComponent());

	StartFollowRadius = 800.0f;
	StopFollowRadius = 1200.0f;
	AttackRadius = 130.0f;

	AcceptanceRadius = 80.0f;

	bIsAttacking = false;

	MaxHealth = 100.0f;

	MinDamage = 10.0f;
	MaxDamage = 25.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	StartFollowSphere->SetSphereRadius(StartFollowRadius);
	StopFollowSphere->SetSphereRadius(StopFollowRadius);
	AttackSphere->SetSphereRadius(AttackRadius);
	AttackSphere->SetRelativeLocation(FVector(AttackRadius, 0.0f, 0.0f));
	AcceptanceRadius = (AttackRadius*2) - GetCapsuleComponent()->GetScaledCapsuleRadius() - 10.0f;

	AIController = Cast<AAIController>(GetController());

	StartFollowSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnStartFollowSphereBeginOverlap);
	StopFollowSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnStopFollowSphereEndOverlap);
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackSphereBeginOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackSphereEndOverlap);

	SetEnemyState(EEnemyState::EES_Idle);

	Health = MaxHealth;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EnemyState == EEnemyState::EES_MovingToTarget && Target && !bIsAttacking)
	{
		MoveToTarget();
	}
	else if (EnemyState == EEnemyState::EES_Attacking && Target && !bIsAttacking)
	{
		Attack();
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnStartFollowSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetEnemyState(EEnemyState::EES_MovingToTarget);
			Target = MainCharacter;
		}
	}
}

void AEnemy::OnStopFollowSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Cast<AMainCharacter>(OtherActor))
		{
			SetEnemyState(EEnemyState::EES_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
			Target = nullptr;
		}
	}
}

void AEnemy::OnAttackSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		if (Cast<AMainCharacter>(OtherActor))
		{
			SetEnemyState(EEnemyState::EES_Attacking);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::OnAttackSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Cast<AMainCharacter>(OtherActor))
		{
			SetEnemyState(EEnemyState::EES_MovingToTarget);
		}
	}
}

void AEnemy::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		FString MontageSection = FString::Printf(TEXT("Attack_%d"), FMath::RandRange(1, 2));
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(*MontageSection, CombatMontage);
	}
}

void AEnemy::Strike()
{
	if (EnemyState == EEnemyState::EES_Attacking && bIsAttacking && Target)
	{
		if (Target->HitParticles)
		{
			FVector SpawnLocation = Target->GetActorLocation();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Target->HitParticles, SpawnLocation, FRotator(0.0f), false);
		}
		if (StrikeSound)
		{
			UGameplayStatics::PlaySound2D(this, StrikeSound);
		}
		if (Target->HitSound)
		{
			UGameplayStatics::PlaySound2D(this, Target->HitSound);
		}
	}
}

void AEnemy::Swing()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
}

void AEnemy::MoveToTarget()
{
	if (AIController)
	{
		AIController->MoveToActor(Target, AcceptanceRadius);
	}
}

