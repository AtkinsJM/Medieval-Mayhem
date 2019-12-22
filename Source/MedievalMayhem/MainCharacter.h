// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pickup.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Walking UMETA(DisplayName = "Walking"),

	EMS_MAX UMETA(DisplayName = "Max")
};

UCLASS()
class MEDIEVALMAYHEM_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;	

	UPROPERTY(EditAnywhere, Category = "Camera")
	float DefaultCameraBoomLength = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class USphereComponent* MeleeCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class USphereComponent* RangedCombatSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MeleeCombatRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float RangedCombatRadius;

	TArray<FVector> PickupLocations;
		   
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementStatus MovementStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AInteractableItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TMap<int32, AWeapon*> Weapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UTexture2D* PrimaryWeaponSetImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UTexture2D* SecondaryWeaponSetImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UTexture2D* NoWeaponSetImage;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Combat")
	class AEnemy* AttackTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	class UParticleSystem* GainHealthParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	class UParticleSystem* GainStaminaParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* GainHealthSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* GainStaminaSound;

	/**
	* PLAYER STATS
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float HealthRegenerationRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaRegenerationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 HealthPotions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 StaminaPotions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float InterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Item Storage")
	TSubclassOf<class AItemStorage> ItemStorage;

	bool bInterpToEnemy;
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void FinishAttack();

	UFUNCTION(BlueprintCallable)
	void StartAttack();

	UFUNCTION(BlueprintCallable)
	void EndDeath();

	UFUNCTION()
	void OnMeleeCombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnMeleeCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRangedCombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnRangedCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void SaveCharacterStats();

	UFUNCTION(BlueprintCallable)
	void LoadCharacterStats();

	void LoadWeaponSets();
		
	void SetMovementStatus(EMovementStatus Status);
	
	void Die();
	void PickUpItem(EPickupType PickupType, FVector Location);
	void IncrementCoins(int32 Amount);

	void PickUpWeapon();
	void DropWeapon();
	void SwapWeaponSet();
	void PickUpWeapon(class AWeapon* Weapon, int32 Index, bool bUseEffects);

	void EquipWeaponSet(int32 Index, bool bUseEffects);

	void UseWeaponSkill(int32 Index);

	void SelectNextEnemy();

	void SetAttackTarget(AEnemy* Target);

	void ConsumePotion(FString PotionType);
	
	void SetOverlappingItem(AInteractableItem* Item);
	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() { return CameraBoom; }
	FORCEINLINE void SetInterpToEnemy(bool Value) { bInterpToEnemy = Value; }
	
	FORCEINLINE AEnemy* GetAttackTarget() { return AttackTarget; }
	FORCEINLINE bool GetIsAlive() { return bIsAlive; }

	
private:
	int32 CurrentWeaponSet;

	bool bIsAlive;

	class UMedievalMayhemGameInstance* GameInstance;
};
