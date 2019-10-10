// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Classes/Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"

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

	MeleeCombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Melee Combat Sphere"));
	MeleeCombatSphere->SetupAttachment(GetRootComponent());

	MeleeCombatRadius = 130.0f;
	
	RangedCombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Ranged Combat Sphere"));
	RangedCombatSphere->SetupAttachment(GetRootComponent());

	RangedCombatRadius = 800.0f;

	MeleeCombatSphere->SetCollisionProfileName("DetectPawnOnly");
	RangedCombatSphere->SetCollisionProfileName("DetectPawnOnly");
	// Set size for capsule
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);

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

	CurrentWeaponSet = 0;

	InterpSpeed = 15.0f;
	bInterpToEnemy = false;
	bIsAlive = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	MeleeCombatSphere->SetSphereRadius(MeleeCombatRadius);
	RangedCombatSphere->SetSphereRadius(RangedCombatRadius);

	MeleeCombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnMeleeCombatSphereBeginOverlap);
	MeleeCombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnMeleeCombatSphereEndOverlap);

	RangedCombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnRangedCombatSphereBeginOverlap);
	RangedCombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnRangedCombatSphereEndOverlap);

	Health = MaxHealth;
	Stamina = MaxStamina;
	
	if (NoWeaponSetImage)
	{
		PrimaryWeaponSetImage = NoWeaponSetImage;
		SecondaryWeaponSetImage = NoWeaponSetImage;
	}

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
	
}

/**
* COLLISION HANDLING
*/
void AMainCharacter::OnMeleeCombatSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (!AttackTarget)
		{
			AttackTarget = Enemy;
			AttackTarget->SetAsTarget(true);
		}
	}
}

void AMainCharacter::OnMeleeCombatSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	/*
	if (EquippedWeapon && OtherActor && AttackTarget)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Melee && Enemy == AttackTarget)
		{
			AttackTarget = nullptr;
			//TODO search for new target
		}
	}
	*/
}

void AMainCharacter::OnRangedCombatSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (EquippedWeapon && OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Ranged && !AttackTarget)
		{
			AttackTarget = Enemy;
			AttackTarget->SetAsTarget(true);
		}
	}
}

void AMainCharacter::OnRangedCombatSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (EquippedWeapon && OtherActor && AttackTarget)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Ranged && Enemy == AttackTarget)
		{
			SelectNextEnemy();
			//AttackTarget = nullptr;
			//TODO search for new target
		}
	}
}

void AMainCharacter::PickUpCoin(FVector Location, int32 Amount)
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

void AMainCharacter::PickUpItem()
{
	if (OverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
		if (Weapon)
		{
			for (int i = 0; i < 2; i++)
			{
				if (!Weapons.Contains(i))
				{
					PickUpWeapon(Weapon, i);
					return;
				}
			}
		}
	}
}

/**
* WEAPON HANDLING
*/
void AMainCharacter::PickUpWeapon(AWeapon * Weapon, int32 Index)
{
	Weapons.Add(Index, Weapon);
	Weapon->PickUp();
	Weapon->SetWeaponInstigator(Controller);
	CurrentWeaponSet = Index;
	EquipWeaponSet(CurrentWeaponSet);
	OverlappingItem = nullptr;
}

void AMainCharacter::DropWeapon()
{
	if (!EquippedWeapon)
	{
		return;
	}
	for (int i = 0; i < 2; i++)
	{
		if (Weapons[i] == EquippedWeapon)
		{
			Weapons.Remove(i);
			break;
		}
	}
	EquippedWeapon->Drop();
	if (NoWeaponSetImage)
	{
		PrimaryWeaponSetImage = NoWeaponSetImage;
	}
	SetEquippedWeapon(nullptr);
}

void AMainCharacter::EquipWeaponSet(int32 Index)
{
	if (Weapons.Contains(Index) && Weapons[Index] != EquippedWeapon && !bIsAttacking)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Unequip();
			SecondaryWeaponSetImage = EquippedWeapon->GetWeaponSetImage();
		}
		SetEquippedWeapon(Weapons[Index]);
		if (EquippedWeapon)
		{
			EquippedWeapon->Equip(this);
			PrimaryWeaponSetImage = EquippedWeapon->GetWeaponSetImage();
		}
	}
}

void AMainCharacter::SwapWeaponSet()
{
	CurrentWeaponSet = CurrentWeaponSet == 0 ? 1 : 0;
	if (Weapons.Contains(CurrentWeaponSet))
	{
		EquipWeaponSet(CurrentWeaponSet);
	}
	else
	{
		CurrentWeaponSet = CurrentWeaponSet == 0 ? 1 : 0;
	}
}


void AMainCharacter::UseWeaponSkill(int32 Index)
{
	if (EquippedWeapon && !bIsAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player using weapon skill %d"), Index);
		StartAttack();
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			FString MontageSection = FString::Printf(TEXT("Attack_%d"), Index);
			AnimInstance->Montage_Play(CombatMontage, 1.3f);
			AnimInstance->Montage_JumpToSection(*MontageSection, CombatMontage);
		}
	}
}

void AMainCharacter::SelectNextEnemy()
{
	TArray<AActor*> EnemiesInRange;
	RangedCombatSphere->GetOverlappingActors(OUT EnemiesInRange, AEnemy::StaticClass());
	
	if (EnemiesInRange.Num() == 0) 
	{ 
		AttackTarget = nullptr;
		return; 
	}
	// TODO: refactor below to only use for loop once?
	if (!AttackTarget)
	{
		int32 NearestEnemyIndex = 0;
		float Distance = (EnemiesInRange[0]->GetTargetLocation() - GetActorLocation()).Size();
		for (size_t i = 1; i < EnemiesInRange.Num(); i++)
		{
			float EnemyDistance = (EnemiesInRange[i]->GetTargetLocation() - GetActorLocation()).Size();
			if (EnemyDistance < Distance)
			{
				Distance = EnemyDistance;
				NearestEnemyIndex = i;
			}
		}
		AttackTarget = Cast<AEnemy>(EnemiesInRange[NearestEnemyIndex]);
		AttackTarget->SetAsTarget(true);
	}
	else
	{
		AttackTarget->SetAsTarget(false);
		for (size_t i = 0; i < EnemiesInRange.Num(); i++)
		{
			if (Cast<AEnemy>(EnemiesInRange[i]) == AttackTarget)
			{
				AttackTarget = Cast<AEnemy>(EnemiesInRange[(i+1) % EnemiesInRange.Num()]);
				AttackTarget->SetAsTarget(true);
				break;
			}
		}
	}
}

FRotator AMainCharacter::GetLookAtRotation(AActor * Target)
{

	return FRotator();
}

/**
* ATTACKING
*/
void AMainCharacter::StartAttack()
{
	bIsAttacking = true;
	bInterpToEnemy = true;
}

void AMainCharacter::FinishAttack()
{
	bIsAttacking = false;
	bInterpToEnemy = false;
}

/**
* DAMAGE HANDLING
*/
float AMainCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->SetTarget(nullptr);
			}
			Die();
		}
	}
	return ActualDamage;
}

void AMainCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage)
	{
		int32 Index = FMath::RandRange(1, 2);
		FString MontageSection = FString::Printf(TEXT("Death_%d"), Index);
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(*MontageSection, CombatMontage);
	}
	bIsAlive = false;
}

void AMainCharacter::EndDeath()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Pause();
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	SetActorEnableCollision(false);
	if (Controller)
	{
		Controller->UnPossess();
	}
}