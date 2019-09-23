// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	TArray<FVector> PickupLocations;
		   
	DECLARE_DELEGATE_OneParam(FWeaponSkillDelegate, int32)
	
	DECLARE_DELEGATE_OneParam(FWeaponSetDelegate, int32)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementStatus MovementStatus;
		
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

	void SetMovementStatus(EMovementStatus Status);
	
	void Die();
	void PickUpCoin(FVector Location, int32 Amount);
	void IncrementCoins(int32 Amount);

	void PickUpItem();
	void DropWeapon();
	void SwapWeaponSet();
	void PickUpWeapon(class AWeapon* Weapon, int32 Index);

	void EquipWeaponSet(int32 Index);

	void UseWeaponSkill(int32 Index);

	FRotator GetLookAtRotation(AActor* Target);

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

	FORCEINLINE void SetOverlappingItem(AInteractableItem* Item) { OverlappingItem = Item; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() { return CameraBoom; }
	FORCEINLINE void SetInterpToEnemy(bool Value) { bInterpToEnemy = Value; }
	FORCEINLINE void SetAttackTarget(AEnemy* Target) { AttackTarget = Target; }
	FORCEINLINE AEnemy* GetAttackTarget() { return AttackTarget; }

	/** 
	* PLAYER STATS 
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float InterpSpeed;

	bool bInterpToEnemy;

	
private:
	int32 CurrentWeaponSet;
};
