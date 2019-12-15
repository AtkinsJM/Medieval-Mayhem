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
#include "MedievalMayhemSaveGame.h"
#include "ItemStorage.h"

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
	HealthPotions = 0;
	StaminaPotions = 0;
	
	RunningSpeed = 450.0f;
	WalkingSpeed = 200.0f;

	CurrentWeaponSet = 0;

	InterpSpeed = 15.0f;
	bInterpToEnemy = false;
	bIsAlive = true;

	bIsSaving = false;
	bIsLoading = false;
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
			SetAttackTarget(Enemy);
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
			SelectNextEnemy();
			//TODO search for new target
		}
	}
}

void AMainCharacter::PickUpItem(EPickupType PickupType, FVector Location)
{
	PickupLocations.Add(Location);

	//UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 12, FLinearColor::Green, 99.9f, 0.5f);
	if (PickupType == EPickupType::EPT_Coin)
	{
		IncrementCoins(1);
	}
	else if (PickupType == EPickupType::EPT_HealthPotion)
	{
		HealthPotions++;
	}
	else if (PickupType == EPickupType::EPT_StaminaPotion)
	{
		StaminaPotions++;
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


void AMainCharacter::UseWeaponSkill(int32 Index)
{
	if (EquippedWeapon && !bIsAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player using weapon skill %d"), Index);
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
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), Stamina);
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

void AMainCharacter::SaveGame()
{
	bIsSaving = true;

	UMedievalMayhemSaveGame* SaveGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.HealthPotions = HealthPotions;
	SaveGameInstance->CharacterStats.StaminaPotions = StaminaPotions;

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	SaveGameInstance->CharacterStats.Weapon1 = Weapons.Contains(0) ? Weapons[0]->Id : "";
	SaveGameInstance->CharacterStats.Weapon2 = Weapons.Contains(1) ? Weapons[1]->Id : "";
	
	SaveGameInstance->CharacterStats.CurrentWeaponSet = CurrentWeaponSet;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SlotName, SaveGameInstance->UserIndex);

	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &AMainCharacter::FinishSaveLoad, 1.0f, true);
}

void AMainCharacter::LoadGame(bool bIsNewLevel)
{
	bIsLoading = true;

	UMedievalMayhemSaveGame* LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::CreateSaveGameObject(UMedievalMayhemSaveGame::StaticClass()));
	LoadGameInstance = Cast<UMedievalMayhemSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SlotName, LoadGameInstance->UserIndex));
	
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Health = LoadGameInstance->CharacterStats.Health;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	Coins = LoadGameInstance->CharacterStats.Coins;
	HealthPotions = LoadGameInstance->CharacterStats.HealthPotions;
	StaminaPotions = LoadGameInstance->CharacterStats.StaminaPotions;

	if (!bIsNewLevel)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	if (ItemStorage)
	{
		AItemStorage* ItemStorageInstance = Cast<AItemStorage>(GetWorld()->SpawnActor<AItemStorage>(ItemStorage));
		if (ItemStorageInstance)
		{
			if (LoadGameInstance->CharacterStats.Weapon1 != "")
			{
				TSubclassOf<AWeapon> WeaponClass = ItemStorageInstance->GetWeapon(LoadGameInstance->CharacterStats.Weapon1);
				if (WeaponClass)
				{
					AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
					PickUpWeapon(Weapon, 0, false);
				}
			}
			if (LoadGameInstance->CharacterStats.Weapon2 != "")
			{
				TSubclassOf<AWeapon> WeaponClass = ItemStorageInstance->GetWeapon(LoadGameInstance->CharacterStats.Weapon2);
				if (WeaponClass)
				{
					AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
					PickUpWeapon(Weapon, 1, false);
				}
			}
		}
		ItemStorageInstance->Destroy();
	}	

	CurrentWeaponSet = LoadGameInstance->CharacterStats.CurrentWeaponSet;
	EquipWeaponSet(CurrentWeaponSet, false);

	GetWorld()->GetTimerManager().SetTimer(SaveLoadTimerHandle, this, &AMainCharacter::FinishSaveLoad, 1.0f, true);
}

void AMainCharacter::FinishSaveLoad()
{
	bIsSaving = bIsLoading = false;
}