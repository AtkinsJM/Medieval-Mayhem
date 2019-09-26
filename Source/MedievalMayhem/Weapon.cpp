// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Texture.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"

#define OUT

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Combat Collision"));
	DamageCollision->SetupAttachment(GetRootComponent());
	
	SetWeaponState(EWeaponState::EWS_Pickup);

	WeaponType = EWeaponType::EWT_Melee;
	WeaponClass = EWeaponClass::EWC_Sword;

	// Shows visible components
	SetActorHiddenInGame(false);
	// Enables collision components
	SetActorEnableCollision(true);
	// Allows the Actor to tick
	SetActorTickEnabled(true);
	
	bRotates = true;
	bFloats = true;

	MinBaseDamage = 10.0f;
	MaxBaseDamage = 20.0f;

	bDealtDamageThisSwing = false;
	bIsSwinging = false;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DamageCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnDamageCollisionBeginOverlap);
	DamageCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnDamageCollisionEndOverlap);

	WeaponSocket = SkeletalMesh->GetSocketByName(FName("WeaponSocket"));
}

void AWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor && WeaponState == EWeaponState::EWS_Pickup)
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if (Character)
		{
			Character->SetOverlappingItem(this);
		}
	}
}

void AWeapon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor && WeaponState == EWeaponState::EWS_Pickup)
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if (Character)
		{
			Character->SetOverlappingItem(nullptr);
		}
	}
}


void AWeapon::Drop()
{
	Destroy();
}


void AWeapon::PickUp()
{
	SetWeaponState(EWeaponState::EWS_Carried);
	//SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	bRotates = false;
	bFloats = false;
	if (IdleParticlesComponent)
	{
		IdleParticlesComponent->Deactivate();
	}
	if (EquipParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipParticles, GetActorLocation(), FRotator(0.0f), true);
	}
}

void AWeapon::Equip(AMainCharacter* Character)
{
	if (Character)
	{
		SetWeaponState(EWeaponState::EWS_Equipped);
		
		// Shows visible components
		SetActorHiddenInGame(false);
		// Enables collision components
		SetActorEnableCollision(true);
		// Allows the Actor to tick
		SetActorTickEnabled(true);

		const USkeletalMeshSocket* MainHandSocket = Character->GetMesh()->GetSocketByName("MainHandSocket");
		if (MainHandSocket)
		{
			MainHandSocket->AttachActor(this, Character->GetMesh());
		}
		
		if (EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}	
	}
}

void AWeapon::Unequip()
{
	SetWeaponState(EWeaponState::EWS_Carried);
	// Hides visible components
	SetActorHiddenInGame(true);
	// Disables collision components
	SetActorEnableCollision(false);
	// Stops the Actor from ticking
	SetActorTickEnabled(false);
}

void AWeapon::OnDamageCollisionBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy && bIsSwinging && !bDealtDamageThisSwing)
		{
			Strike(Enemy);
		}
	}	
}

void AWeapon::Strike(AEnemy* Enemy)
{
	if (DamageTypeClass && WeaponInstigator)
	{
		float Damage = FMath::RandRange(MinBaseDamage, MaxBaseDamage);
		UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
	}
	if (StrikeParticles)
	{
		FVector SpawnLocation = WeaponSocket ? WeaponSocket->GetSocketLocation(SkeletalMesh) : DamageCollision->GetComponentLocation();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), StrikeParticles, SpawnLocation, FRotator(0.0f), false);
	}
	if (Enemy->HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, Enemy->GetActorLocation(), FRotator(0.0f), false);
	}
	if (StrikeSound)
	{
		UGameplayStatics::PlaySound2D(this, StrikeSound);
	}
	if (Enemy->HitSound)
	{
		UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
	}
	bDealtDamageThisSwing = true;
}

void AWeapon::StartSwing()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
	bDealtDamageThisSwing = false;
	bIsSwinging = true;
}

void AWeapon::EndSwing()
{
	bIsSwinging = false;
}


void AWeapon::OnDamageCollisionEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}