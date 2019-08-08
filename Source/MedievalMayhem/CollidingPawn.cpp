// Fill out your copyright notice in the Description page of Project Settings.


#include "CollidingPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CollidingPawnMovementComponent.h"

// Sets default values
ACollidingPawn::ACollidingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		MeshComponent->SetWorldScale3D(FVector(0.8f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->RelativeRotation = FRotator(-45.0f, 0.0f, 0.0f);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UCollidingPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = GetRootComponent();

	CameraInput = FVector2D(0.0f, 0.0f);
	CameraZoomSpeed = 10.0f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollidingPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollidingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanRotateCamera)
	{
		RotateCamera();
	}	
}

// Called to bind functionality to input
void ACollidingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ACollidingPawn::MoveX);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ACollidingPawn::MoveY);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollidingPawn::YawCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollidingPawn::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraZoom"), this, &ACollidingPawn::ZoomCamera);

	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Pressed, this, &ACollidingPawn::EnableCameraRotation);
	PlayerInputComponent->BindAction(TEXT("RotateCamera"), EInputEvent::IE_Released, this, &ACollidingPawn::DisableCameraRotation);
}

UPawnMovementComponent* ACollidingPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void ACollidingPawn::MoveX(float Value)
{
	FVector Forward = GetActorForwardVector();
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(Forward * Value * 150.0f);
	}
}

void ACollidingPawn::MoveY(float Value)
{
	FVector Right = GetActorRightVector();
	if (MovementComponent)
	{
		MovementComponent->AddInputVector(Right * Value * 150.0f);
	}
}

void ACollidingPawn::YawCamera(float Value)
{
	CameraInput.X = Value;
}

void ACollidingPawn::PitchCamera(float Value)
{
	CameraInput.Y = Value;
}

void ACollidingPawn::ZoomCamera(float Value)
{
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength - (Value * CameraZoomSpeed), 200.0f, 600.0f);
}

void ACollidingPawn::EnableCameraRotation()
{
	bCanRotateCamera = true;
}

void ACollidingPawn::DisableCameraRotation()
{
	bCanRotateCamera = false;
}

void ACollidingPawn::RotateCamera()
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArm->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -75.0f, 0.0f);
	SpringArm->SetWorldRotation(NewSpringArmRotation);
}

