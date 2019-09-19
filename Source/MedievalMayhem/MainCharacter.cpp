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
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

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

float AMainCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			Die();
		}
	}
	return ActualDamage;
}

void AMainCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died!"));
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	
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

void AMainCharacter::FinishAttack()
{
	bIsAttacking = false;
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

void AMainCharacter::PickUpWeapon(AWeapon * Weapon, int32 Index)
{
	Weapons.Add(Index, Weapon);
	Weapon->PickUp();
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

void AMainCharacter::UseWeaponSkill(int32 Index)
{
	if (EquippedWeapon && !bIsAttacking)
	{
		bIsAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			FString MontageSection = FString::Printf(TEXT("Attack_%d"), Index);
			AnimInstance->Montage_Play(CombatMontage, 1.3f);
			AnimInstance->Montage_JumpToSection(*MontageSection, CombatMontage);
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