// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.h"
#include "MainCharacterController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Classes/Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar"));
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
	HealthBar->SetDrawSize(FVector2D(120.0f, 30.0f));
	HealthBar->SetVisibility(false);

	TargetCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target Circle"));
	TargetCircle->SetupAttachment(GetRootComponent());

	StartFollowSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Start Follow Sphere"));
	StartFollowSphere->SetupAttachment(GetRootComponent());
		
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

	DestroyDelay = 1.0f;

	DisplayName = "Enemy";

	bFinishedSpawning = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (!MainCharacter)
	{
		MainCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	}

	TargetCircle->SetVisibility(false);
	
	StartFollowSphere->SetSphereRadius(StartFollowRadius);
	StopFollowSphere->SetSphereRadius(StopFollowRadius);
	CombatSphere->SetSphereRadius(MeleeCombatRadius);
	AttackSphere->SetSphereRadius(AttackRadius);
	AttackSphere->SetRelativeLocation(FVector(AttackRadius, 0.0f, 0.0f));
	AcceptanceRadius = (AttackRadius*2) - GetCapsuleComponent()->GetScaledCapsuleRadius() - 10.0f;

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
	if (!IsAlive()) { return; }

	if (HealthBar)
	{		
		//TODO: hook up health bar properly, and destroy on death
		FRotator WantedRotation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();
		WantedRotation.Pitch *= -1;
		WantedRotation.Yaw += 180;
		HealthBar->SetWorldRotation(WantedRotation);
	}

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
				FRotator WantedRotation = GetActorRotation();
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
				FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 10.0f);
				WantedRotation.Yaw = InterpRotation.Yaw;
				SetActorRotation(WantedRotation);
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

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AIController = Cast<AAIController>(NewController);
	if (HealthBar)
	{
		HealthBar->SetVisibility(true);
	}
}

/**
* COLLISION HANDLING
*/
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
			FRotator WantedRotation = GetActorRotation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
			WantedRotation.Yaw = LookAtRotation.Yaw;
			SetActorRotation(WantedRotation);
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

void AEnemy::FinishSpawnAndPossess()
{
	bFinishedSpawning = true;
	if (!AIController)
	{
		SpawnDefaultController();
	}
}

void AEnemy::MoveToTarget()
{
	if (AIController)
	{
		AIController->MoveToActor(Target, AcceptanceRadius);
	}
}

/**
* ATTACKING
*/
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
		if (DamageTypeClass != NULL)
		{
			float Damage = FMath::RandRange(MinDamage, MaxDamage);
			UGameplayStatics::ApplyDamage(AttackTarget, Damage, AIController, this, DamageTypeClass);
		}
	}
}

void AEnemy::EndAttack()
{
	bIsAttacking = false;
	LastAttackTime = GetWorld()->GetTimeSeconds();
	CurrentAttackDelay = FMath::RandRange(MinAttackDelay, MaxAttackDelay);
}

/**
* DAMAGE HANDLING
*/
float AEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			AMainCharacterController* MainCharacterController = Cast<AMainCharacterController>(EventInstigator);
			if (MainCharacterController)
			{
				AMainCharacter* MainCharacter = MainCharacterController->GetMainCharacter();
				if (MainCharacter)
				{
					if (this == MainCharacter->GetAttackTarget())
					{
						SetAsTarget(false);
						MainCharacter->SetAttackTarget(nullptr);
					}
				}
			}
			Die();
		}
	}
	return ActualDamage;
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySound2D(this, DeathSound);
	}
	HealthBar->DestroyComponent();
	SetEnemyState(EEnemyState::EES_Dead);
}

bool AEnemy::IsAlive()
{
	return EnemyState != EEnemyState::EES_Dead;
}

void AEnemy::EndDeath()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Pause();
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	SetActorEnableCollision(false);
	if (AIController)
	{
		AIController->UnPossess();
	}
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::DestroyEnemy, DestroyDelay);
}

void AEnemy::DestroyEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed!"));
	Destroy();
}

void AEnemy::SetAsTarget(bool State)
{
	TargetCircle->SetVisibility(State);
}
