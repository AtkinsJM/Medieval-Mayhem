// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Idle UMETA(DisplayName = "Idle"),
	EES_MovingToTarget UMETA(DisplayName = "MovingToTarget"),
	EES_Attacking UMETA(DisplayName = "Attacking"),

	EES_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class MEDIEVALMAYHEM_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	EEnemyState EnemyState;

	FORCEINLINE void SetEnemyState(EEnemyState State) { EnemyState = State; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	/* Volume within which the enemy starts to follow the player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* StartFollowSphere;

	/* Volume outside of which the enemy stops following the player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* StopFollowSphere;
	
	/* Volume in which the enemy is in combat with the player and can attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	/* Volume in which the player must be in for the enemy to attack*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AttackSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float StartFollowRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float StopFollowRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float MeleeCombatRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float AttackRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Properties")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy | Properties")
	float MinDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy | Properties")
	float MaxDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy | Properties")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Sounds")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Sounds")
	USoundCue* AttackSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Sounds")
	USoundCue* StrikeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Sounds")
	USoundCue* DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* CombatMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MinAttackDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxAttackDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AMainCharacter* AttackTarget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnStartFollowSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnStopFollowSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnMeleeCombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnMeleeCombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnAttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnAttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Attack();

	UFUNCTION(BlueprintCallable)
	void Strike();
	
	UFUNCTION(BlueprintCallable)
	void Swing();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	void MoveToTarget();

private:
	class AMainCharacter* Target;
	float AcceptanceRadius;

	float Health;

	bool bInterpToTarget;
	
	float LastAttackTime;
	float CurrentAttackDelay;
};
