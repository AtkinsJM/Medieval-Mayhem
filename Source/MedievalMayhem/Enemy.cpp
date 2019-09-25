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
#include "Kismet/KismetMathLibrary.h"

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

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Melee Combat Sphere"));
	CombatSphere->SetupAttachment(GetRootComponent());

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(GetRootComponent());

	StartFollowSphere->SetCollisionProfileName("DetectPawnOnly");
	StopFollowSphere->SetCollisionProfileName("DetectPawnOnly");
	CombatSphere->SetCollisionProfileName("DetectPawnOnly");
	AttackSphere->SetCollisionProfileName("DetectPawnOnly");

	StartFollowRadius = 800.0f;
	StopFollowRadius = 1200.0f;
	MeleeCombatRadius = 130.0f;
	AttackRadius = 65.0f;

	AcceptanceRadius = 80.0f;

	bIsAttacking = false;

	MaxHealth = 100.0f;

	MinDamage = 10.0f;
	MaxDamage = 25.0f;

	bInterpToTarget = false;

	EnemyState = EEnemyState::EES_Idle;

	MinAttackDelay = 2.0f;
	MaxAttackDelay = 3.0f;

	LastAttackTime = 0.0f;
	CurrentAttackDelay = 0.0f;

	AttackTarget = nullptr;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	StartFollowSphere->SetSphereRadius(StartFollowRadius);
	StopFollowSphere->SetSphereRadius(StopFollowRadius);
	CombatSphere->SetSphereRadius(MeleeCombatRadius);
	AttackSphere->SetSphereRadius(AttackRadius);
	AttackSphere->SetRelativeLocation(FVector(AttackRadius, 0.0f, 0.0f));
	AcceptanceRadius = (AttackRadius*2) - GetCapsuleComponent()->GetScaledCapsuleRadius() - 10.0f;

	/*StartFollowSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StartFollowSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StartFollowSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StartFollowSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	StopFollowSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StopFollowSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StopFollowSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StopFollowSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CombatSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	*/
	AIController = Cast<AAIController>(GetController());

	StartFollowSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnStartFollowSphereBeginOverlap);
	StopFollowSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnStopFollowSphereEndOverlap);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnMeleeCombatSphereBeginOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnMeleeCombatSphereEndOverlap);
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
		
		if ((GetWorld()->GetTimeSeconds() - LastAttackTime) > CurrentAttackDelay)
		{
			if (!AttackTarget)
			{
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
				FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 10.0f);
				SetActorRotation(InterpRotation);
			}
			else
			{
				Attack();
			}
		}	
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

void AEnemy::OnMeleeCombatSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetEnemyState(EEnemyState::EES_Attacking);
			
		}
	}
}

void AEnemy::OnMeleeCombatSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetEnemyState(EEnemyState::EES_MovingToTarget);
		}
	}
}

void AEnemy::OnAttackSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			AttackTarget = MainCharacter;
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MainCharacter->GetActorLocation());
			SetActorRotation(LookAtRotation);
		}
	}
}

void AEnemy::OnAttackSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && AttackTarget)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter == AttackTarget)
			{
				AttackTarget = nullptr;
			}
		}
	}
}

void AEnemy::Attack()
{
	bIsAttacking = true;
	if (AIController)
	{
		AIController->StopMovement();
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		FString MontageSection = FString::Printf(TEXT("Attack_%d"), FMath::RandRange(1, 2));
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(*MontageSection, CombatMontage);
	}
}

void AEnemy::Swing()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
}

void AEnemy::Strike()
{
	if (EnemyState == EEnemyState::EES_Attacking && bIsAttacking && AttackTarget)
	{
		if (AttackTarget->HitParticles)
		{
			FVector SpawnLocation = AttackTarget->GetActorLocation();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackTarget->HitParticles, SpawnLocation, FRotator(0.0f), false);
		}
		if (StrikeSound)
		{
			UGameplayStatics::PlaySound2D(this, StrikeSound);
		}
		if (AttackTarget->HitSound)
		{
			UGameplayStatics::PlaySound2D(this, AttackTarget->HitSound);
		}
	}
}

void AEnemy::EndAttack()
{
	bIsAttacking = false;
	LastAttackTime = GetWorld()->GetTimeSeconds();
	CurrentAttackDelay = FMath::RandRange(MinAttackDelay, MaxAttackDelay);
}

void AEnemy::MoveToTarget()
{
	if (AIController)
	{
		AIController->MoveToActor(Target, AcceptanceRadius);

	}
}

