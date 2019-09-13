// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	//SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
	//SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SetWeaponState(EWeaponState::EWS_Pickup);

	// Shows visible components
	SetActorHiddenInGame(false);
	// Enables collision components
	SetActorEnableCollision(true);
	// Allows the Actor to tick
	SetActorTickEnabled(true);
	
	bRotates = true;
	bFloats = true;
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
