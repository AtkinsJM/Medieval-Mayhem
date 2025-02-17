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
#include "Pickup.h"
#include "ItemStorage.h"
#include "MedievalMayhemGameInstance.h"

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
	HealthRegenerationRate = 1.0f;
	MaxStamina = 100.0f;
	Stamina = 100.0f;
	StaminaRegenerationRate = 1.0f;
	Coins = 0;
	HealthPotions = 0;
	StaminaPotions = 0;
	
	RunningSpeed = 450.0f;
	WalkingSpeed = 200.0f;

	CurrentWeaponSet = 0;

	InterpSpeed = 15.0f;
	bInterpToEnemy = false;
	bIsAlive = true;
	
	GameInstance = nullptr;
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
	
	GameInstance = Cast<UMedievalMayhemGameInstance>(GetGameInstance());
	if (GameInstance)
	{		
		if (GameInstance->bIsNewGame)
		{
			//On new game start up, initialise GameInstance CharacterStats struct with player's stats.
			SaveCharacterStats();
			GameInstance->bIsNewGame = false;
		}
		else
		{
			if (GameInstance->bIsTransitioning)
			{
				GameInstance->bIsTransitioning = false;
			}
			// If not a new game and level loaded, get correct stats from GameInstance
			LoadCharacterStats();
			if (GameInstance->bIsNewLevel)
			{
				GameInstance->SaveGame("Autosave");
				GameInstance->bIsNewLevel = false;
			}
		}	
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAlive) { return; }

	// If not in combat, regenerate health
	if (!AttackTarget)
	{
		Health = FMath::Clamp(Health + (HealthRegenerationRate * DeltaTime), 0.0f, MaxHealth);
	}
	// Regenerate stamina
	Stamina = FMath::Clamp(Stamina + (StaminaRegenerationRate * DeltaTime), 0.0f, MaxStamina);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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
			SetAttackTarget(Enemy);
		}
	}
}

void AMainCharacter::OnMeleeCombatSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}

void AMainCharacter::OnRangedCombatSphereBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (EquippedWeapon && OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Ranged && !AttackTarget)
		{
			SetAttackTarget(Enemy);
		}
	}
}

void AMainCharacter::OnRangedCombatSphereEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && AttackTarget)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		//if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Ranged && Enemy == AttackTarget)
		if (Enemy == AttackTarget)
		{
			SetAttackTarget(nullptr);
			SelectNextEnemy();
		}
	}
}

void AMainCharacter::PickUpItem(EPickupType PickupType, FVector Location)
{
	PickupLocations.Add(Location);

	switch (PickupType)
	{
		case EPickupType::EPT_Coin:
			IncrementCoins(1);
			break;
		case EPickupType::EPT_HealthPotion:
			HealthPotions++;
			break;
		case EPickupType::EPT_StaminaPotion:
			StaminaPotions++;
			break;
		default:
			break;
	}
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

void AMainCharacter::SetOverlappingItem(AInteractableItem* Item) 
{ 
	OverlappingItem = Item;
	// TODO some functionality of interaction widget into method?
	if (OverlappingItem)
	{
		// If weapon
		if (Cast<AWeapon>(OverlappingItem))
		{

		}
	}
}

void AMainCharacter::PickUpWeapon()
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
					PickUpWeapon(Weapon, i, true);
					return;
				}
			}
		}
	}
}

/**
* WEAPON HANDLING
*/
void AMainCharacter::PickUpWeapon(AWeapon * Weapon, int32 Index, bool bUseEffects = true)
{
	Weapons.Add(Index, Weapon);
	Weapon->PickUp(bUseEffects);
	Weapon->SetWeaponInstigator(Controller);
	CurrentWeaponSet = Index;
	EquipWeaponSet(CurrentWeaponSet, bUseEffects);
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

void AMainCharacter::EquipWeaponSet(int32 Index, bool bUseEffects = true)
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
			EquippedWeapon->Equip(this, bUseEffects);
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

bool AMainCharacter::CanUseWeaponSkill(int32 Index) 
{ 
	if (!EquippedWeapon) { return false; }
	if (Index > EquippedWeapon->WeaponSkillStaminaRequirements.Num()) { return false; }
	return Stamina > EquippedWeapon->WeaponSkillStaminaRequirements[Index - 1];
}

void AMainCharacter::UseWeaponSkill(int32 Index)
{
	if (EquippedWeapon && !bIsAttacking)
	{
		Stamina -= EquippedWeapon->WeaponSkillStaminaRequirements[Index-1];
		StartAttack();
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if (Index == 1)
			{
				int rand = FMath::RandRange(0, 2);
				Index = 10 + rand;
			}
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
		SetAttackTarget(nullptr);
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
		SetAttackTarget(Cast<AEnemy>(EnemiesInRange[NearestEnemyIndex]));
	}
	else
	{
		for (size_t i = 0; i < EnemiesInRange.Num(); i++)
		{
			if (Cast<AEnemy>(EnemiesInRange[i]) == AttackTarget)
			{
				SetAttackTarget(Cast<AEnemy>(EnemiesInRange[(i + 1) % EnemiesInRange.Num()]));
				break;
			}
		}
	}
}

void AMainCharacter::SetAttackTarget(AEnemy* Target)
{
	if (AttackTarget)
	{
		AttackTarget->SetAsTarget(false);
	}
	AttackTarget = Target;
	if(AttackTarget)
	{
		AttackTarget->SetAsTarget(true);
	}
}

void AMainCharacter::ConsumePotion(FString PotionType)
{
	if (PotionType == "Health Potion")
	{
		if (HealthPotions > 0)
		{
			HealthPotions--;
			Health = FMath::Clamp(Health + 25, 0.0f, MaxHealth);

			if (GainHealthParticles)
			{
				FVector SpawnLocation = GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GainHealthParticles, SpawnLocation, FRotator(0.0f), false);
			}
			if (GainHealthSound)
			{
				UGameplayStatics::PlaySound2D(this, GainHealthSound);
			}
		}
	}
	else if (PotionType == "Stamina Potion")
	{
		if (StaminaPotions > 0)
		{
			StaminaPotions--;
			Stamina = FMath::Clamp(Stamina + 25, 0.0f, MaxStamina);

			if (GainStaminaParticles)
			{
				FVector SpawnLocation = GetActorLocation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GainStaminaParticles, SpawnLocation, FRotator(0.0f), false);
			}
			if (GainStaminaSound)
			{
				UGameplayStatics::PlaySound2D(this, GainStaminaSound);
			}
		}
	}
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

void AMainCharacter::SaveCharacterStats()
{
	GameInstance->CharacterStats.MaxHealth = MaxHealth;
	GameInstance->CharacterStats.Health = Health;
	GameInstance->CharacterStats.Stamina = Stamina;
	GameInstance->CharacterStats.MaxStamina = MaxStamina;
	GameInstance->CharacterStats.Coins = Coins;
	GameInstance->CharacterStats.HealthPotions = HealthPotions;
	GameInstance->CharacterStats.StaminaPotions = StaminaPotions;

	GameInstance->CharacterStats.Location = GetActorLocation();
	GameInstance->CharacterStats.Rotation = GetActorRotation();

	GameInstance->CharacterStats.Weapon1 = Weapons.Contains(0) ? Weapons[0]->Id : "";
	GameInstance->CharacterStats.Weapon2 = Weapons.Contains(1) ? Weapons[1]->Id : "";

	GameInstance->CharacterStats.CurrentWeaponSet = CurrentWeaponSet;
}

void AMainCharacter::LoadCharacterStats()
{
	MaxHealth = GameInstance->CharacterStats.MaxHealth;
	Health = GameInstance->CharacterStats.Health;
	MaxStamina = GameInstance->CharacterStats.MaxStamina;
	Stamina = GameInstance->CharacterStats.Stamina;
	Coins = GameInstance->CharacterStats.Coins;
	HealthPotions = GameInstance->CharacterStats.HealthPotions;
	StaminaPotions = GameInstance->CharacterStats.StaminaPotions;

	if (!GameInstance->bIsNewLevel)
	{
		SetActorLocation(GameInstance->CharacterStats.Location);
		SetActorRotation(GameInstance->CharacterStats.Rotation);
	}

	LoadWeaponSets();	
}

void AMainCharacter::LoadWeaponSets()
{
	CurrentWeaponSet = GameInstance->CharacterStats.CurrentWeaponSet;
	if (ItemStorage)
	{
		AItemStorage* ItemStorageInstance = Cast<AItemStorage>(GetWorld()->SpawnActor<AItemStorage>(ItemStorage));
		if (ItemStorageInstance)
		{
			if (GameInstance->CharacterStats.Weapon1 != "")
			{
				TSubclassOf<AWeapon> WeaponClass = ItemStorageInstance->GetWeapon(GameInstance->CharacterStats.Weapon1);
				if (WeaponClass)
				{
					AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
					PickUpWeapon(Weapon, 0, false);
				}
			}
			if (GameInstance->CharacterStats.Weapon2 != "")
			{
				TSubclassOf<AWeapon> WeaponClass = ItemStorageInstance->GetWeapon(GameInstance->CharacterStats.Weapon2);
				if (WeaponClass)
				{
					AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
					PickUpWeapon(Weapon, 1, false);
				}
			}
		}
		ItemStorageInstance->Destroy();
	}
	EquipWeaponSet(CurrentWeaponSet, false);
}
