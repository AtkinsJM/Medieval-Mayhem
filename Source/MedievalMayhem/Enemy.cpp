// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"

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
	AttackRadius = 120.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	StartFollowSphere->SetSphereRadius(StartFollowRadius);
	StopFollowSphere->SetSphereRadius(StopFollowRadius);
	AttackSphere->SetSphereRadius(AttackRadius);

	AIController = Cast<AAIController>(GetController());

	StartFollowSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnStartFollowSphereBeginOverlap);
	StopFollowSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnStopFollowSphereEndOverlap);
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackSphereBeginOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackSphereEndOverlap);

	SetEnemyState(EEnemyState::EES_Idle);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		if (Cast<AMainCharacter>(OtherActor))
		{
			MoveToTarget(OtherActor);
		}
	}
}

void AEnemy::OnStopFollowSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	SetEnemyState(EEnemyState::EES_Idle);
}

void AEnemy::OnAttackSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	SetEnemyState(EEnemyState::EES_Attacking);
}

void AEnemy::OnAttackSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	SetEnemyState(EEnemyState::EES_MovingToTarget);
}

void AEnemy::MoveToTarget(AActor * Target)
{
	SetEnemyState(EEnemyState::EES_MovingToTarget);

	if (AIController)
	{
		AIController->MoveToActor(Target, AttackRadius);
	}
}

