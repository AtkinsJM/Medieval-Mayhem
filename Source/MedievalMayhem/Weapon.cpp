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

	WeaponState = EWeaponState::EWS_Pickup;
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

void AWeapon::Equip(AMainCharacter* Character)
{
	if (Character)
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetSimulatePhysics(false);
		bRotates = false;
		bFloats = false;
		const USkeletalMeshSocket* MainHandSocket = Character->GetMesh()->GetSocketByName("MainHandSocket");
		if (MainHandSocket)
		{
			MainHandSocket->AttachActor(this, Character->GetMesh());
		}
		if (EquipParticles != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipParticles, GetActorLocation(), FRotator(0.0f), true);
		}
		if (EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}
		WeaponState = EWeaponState::EWS_Equipped;
	}
}