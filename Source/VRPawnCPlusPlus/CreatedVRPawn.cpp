// Fill out your copyright notice in the Description page of Project Settings.
#pragma warning(disable:4996)

#include "CreatedVRPawn.h"
#include "MotionControllerComponent.h"
#include "CineCameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

// Sets default values
ACreatedVRPawn::ACreatedVRPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize VR origin component
	VROriginComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VROriginComponent"));
	RootComponent = VROriginComponent;

	// Initialize camera component
	CameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(VROriginComponent);

	// Initialize right motion controller component
	MotionControllerComponentRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerComponentRight"));
	MotionControllerComponentRight->SetTrackingSource(EControllerHand::Right);
	MotionControllerComponentRight->SetupAttachment(VROriginComponent);

	// Initialize right controller mesh component
	RightControllerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightControllerMeshComponent"));
	RightControllerMeshComponent->SetupAttachment(MotionControllerComponentRight);
	RightControllerMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	// Initialize left motion controller component
	MotionControllerComponentLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerComponentLeft"));
	MotionControllerComponentLeft->SetTrackingSource(EControllerHand::Left);
	MotionControllerComponentLeft->SetupAttachment(VROriginComponent);

	// Initialize left controller mesh component
	LeftControllerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftControllerMeshComponent"));
	LeftControllerMeshComponent->SetupAttachment(MotionControllerComponentLeft);
	LeftControllerMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	// Load and set controller mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ControllerMeshAsset(
		TEXT("/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid"));
	if (ControllerMeshAsset.Succeeded())
	{
		RightControllerMeshComponent->SetStaticMesh(ControllerMeshAsset.Object);
		LeftControllerMeshComponent->SetStaticMesh(ControllerMeshAsset.Object);
	}

	// Load and set controller material
	static ConstructorHelpers::FObjectFinder<UMaterial> ControllerMaterialAsset(
		TEXT("/Game/StarterContent/Materials/M_Basic_Wall.M_Basic_Wall"));
	if (ControllerMaterialAsset.Succeeded())
	{
		RightControllerMeshComponent->SetMaterial(0, ControllerMaterialAsset.Object);
		LeftControllerMeshComponent->SetMaterial(0, ControllerMaterialAsset.Object);
	}

	// Disable collision for controller mesh components
	RightControllerMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftControllerMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set this pawn to auto possess player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACreatedVRPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
			
			(PlayerController->GetLocalPlayer()))
		{
			// Ensure your Input Mapping Context is assigned
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ACreatedVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACreatedVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to UEnhancedInputComponent and bind input actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ACreatedVRPawn::MoveForward);
		EnhancedInputComponent->BindAction(SnapTurnAction, ETriggerEvent::Triggered, this, &ACreatedVRPawn::SnapTurn);
		EnhancedInputComponent->BindAction(FlyUpAction, ETriggerEvent::Triggered, this, &ACreatedVRPawn::FlyUp);
	}
}

// Function to move the pawn forward/backward
void ACreatedVRPawn::MoveForward(const FInputActionValue& Value)
{
	// Retrieve input value
	float ForwardValue = Value.Get<float>();

	if (ForwardValue != 0.0f)
	{
		// Get the forward vector of the camera
		FVector Direction = CameraComponent->GetForwardVector();

		// Zero out the Z component to keep movement on the ground
		Direction.Z = 0.0f;
		Direction.Normalize();

		// Apply movement in the horizontal plane (ground level)
		AddActorWorldOffset(Direction * ForwardValue * Speed * GetWorld()->GetDeltaSeconds(), true);
	}
}

// Function to turn the pawn left/right
void ACreatedVRPawn::SnapTurn(const FInputActionValue& Value)
{
	// Retrieve input value and rotate the actor
	float TurnValue = Value.Get<float>();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Turn Value: %f"), TurnValue));
	}
	AddActorWorldRotation(FRotator(0, TurnValue * TurnRate, 0));
}

// Function to move the pawn up/down
void ACreatedVRPawn::FlyUp(const FInputActionValue& Value)
{
	float FlyInputValue = Value.Get<float>();

	if (FlyInputValue != 0.0f)
	{
		// Get the forward vector of the camera
		FVector FlyDirection = CameraComponent->GetForwardVector();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Fly Up Value: %f"), FlyInputValue));
		}
		// Apply movement in the direction the camera is facing
		AddActorWorldOffset(FlyDirection * FlyRate * FlyInputValue* GetWorld()->GetDeltaSeconds(), true);
	}
}
