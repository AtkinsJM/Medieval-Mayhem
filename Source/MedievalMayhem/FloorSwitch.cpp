// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 20.0f));

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch Mesh"));
	SwitchMesh->SetupAttachment(GetRootComponent());

	//DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	//DoorMesh->SetupAttachment(GetRootComponent());

	CloseDelay = 1.0f;

	bSwitchDepressed = false;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnEndOverlap);

	//InitialDoorLocation = DoorMesh->GetComponentLocation();
	InitialDoorLocation = Object->GetActorLocation();
	InitialSwitchLocation = SwitchMesh->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Object != nullptr)
	{
		RaiseDoor();
		LowerFloorSwitch();
		bSwitchDepressed = true;
	}	
}

void AFloorSwitch::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Object != nullptr)
	{
		RaiseFloorSwitch();
		bSwitchDepressed = false;
		GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::LowerDoor, CloseDelay);
	}	
}

void AFloorSwitch::UpdateDoorLocation(FVector Displacement)
{
	FVector NewLocation = InitialDoorLocation + Displacement;
	//DoorMesh->SetWorldLocation(NewLocation);
	Object->SetActorLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(FVector Displacement)
{
	FVector NewLocation = InitialSwitchLocation + Displacement;
	SwitchMesh->SetWorldLocation(NewLocation);
}