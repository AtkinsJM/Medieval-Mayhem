// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Carried UMETA(DisplayName = "Carried"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Melee UMETA(DisplayName = "Melee"),
	EWT_Ranged UMETA(DisplayName = "Ranged"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	EWC_Sword UMETA(DisplayName = "Sword"),
	EWC_Axe UMETA(DisplayName = "Axe"),
	EWC_Hammer UMETA(DisplayName = "Hammer"),
	EWC_Club UMETA(DisplayName = "Club"),
	EWC_Staff UMETA(DisplayName = "Staff"),

	EWC_MAX UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class MEDIEVALMAYHEM_API AWeapon : public AInteractableItem
{
	GENERATED_BODY()
	AWeapon();

public:

	EWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Properties")
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Properties")
	EWeaponClass WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Properties")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystem* EquipParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	UParticleSystem* StrikeParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Image")
	class UTexture2D* Image;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Combat")
	class UBoxComponent* DamageCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Combat")
	float MinBaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Combat")
	float MaxBaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Combat")
	TArray<int32> WeaponSkillStaminaRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	USoundCue* StrikeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Properties")
	AController* WeaponInstigator;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void PickUp(bool bUseEffects);
	void Drop();
	void Equip(class AMainCharacter* Character, bool bUseEffects);
	void Unequip();
	
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }
	FORCEINLINE UTexture2D* GetWeaponSetImage() { return Image; }
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }
	FORCEINLINE EWeaponClass GetWeaponClass() { return WeaponClass; }
	FORCEINLINE void SetWeaponInstigator(AController* Instigator) { WeaponInstigator = Instigator; }

	UFUNCTION()
	void OnDamageCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnDamageCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void Strike(class AEnemy* Enemy);

	UFUNCTION(BlueprintCallable)
	void StartSwing();

	UFUNCTION(BlueprintCallable)
	void EndSwing();

private:
	const class USkeletalMeshSocket* WeaponSocket;

	bool bIsSwinging;
	bool bDealtDamageThisSwing;
};
