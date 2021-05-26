// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WackyNoodleCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "WN_BaseGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WN_BaseGameMode.h"
//////////////////////////////////////////////////////////////////////////
// AWackyNoodleCharacter

AWackyNoodleCharacter::AWackyNoodleCharacter()
{
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input



void AWackyNoodleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("LookBehind", IE_Pressed, this, &AWackyNoodleCharacter::lookBehind);
	PlayerInputComponent->BindAction("LookBehind", IE_Released, this, &AWackyNoodleCharacter::lookBehind);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWackyNoodleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWackyNoodleCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &AWackyNoodleCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AWackyNoodleCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWackyNoodleCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWackyNoodleCharacter::TouchStopped);
}

void AWackyNoodleCharacter::UpdatePhase()
{


	if (bIsBoosting)
	{
		CurrentPhase = EPhase::BOOST;
		return;
	}

	CurrentPhase = EPhase::PHASE0;

	if (CurrentSpeed >= FLT_EPSILON)
		CurrentPhase = EPhase::PHASE1;
	if (CurrentSpeed >= PhaseMap[EPhase::PHASE2])
		CurrentPhase = EPhase::PHASE2;
	if (CurrentSpeed >= PhaseMap[EPhase::PHASE3])
		CurrentPhase = EPhase::PHASE3;
}

void AWackyNoodleCharacter::resetJumpTimer()
{
	if (HasAuthority())
		bCanJump = true;
}

EPhase AWackyNoodleCharacter::GetNextPhase()
{
	switch (CurrentPhase)
	{
	case EPhase::PHASE0:
		return EPhase::PHASE1;
		break;
	case EPhase::PHASE1:
		return EPhase::PHASE2;
		break;
	case EPhase::PHASE2:
		return EPhase::PHASE3;
		break;
	case EPhase::PHASE3:
		return EPhase::BOOST;
		break;
	case EPhase::BOOST:
		return EPhase::BOOST;
		break;
	default:
		return EPhase::PHASE1;
		break;
	}
}

void AWackyNoodleCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void AWackyNoodleCharacter::Jump()
{
	if (bCanJump)
	{
		Super::Jump();
		if (HasAuthority())
		{
			bCanJump = false;
			FTimerHandle jumpHandle;
			GetWorld()->GetTimerManager().SetTimer(jumpHandle, this, &AWackyNoodleCharacter::resetJumpTimer, jumpDelay,
			                                       false);
		}
	}
}

void AWackyNoodleCharacter::ServerUpdateRecieveInput_Implementation(float Value)
{
	if (Value <= 0.f)
		bReceivingInput = false;
	else
		bReceivingInput = true;

	if (bAutoSlowDown == false && Value < 0.f)
		slowDown = true;
	else
		slowDown = false;
}

bool AWackyNoodleCharacter::ServerUpdateRecieveInput_Validate(float Value)
{
	return true;
}


void AWackyNoodleCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if(HasAuthority())
	{
		UpdatePhase();
		updateSpeed(DeltaSeconds);
		GetCharacterMovement()->MaxWalkSpeed = PhaseMap[CurrentPhase];
		LowerPlayerPhase();

	}

}

void AWackyNoodleCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AWackyNoodleCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AWackyNoodleCharacter::updateSpeed(float DeltaTime)
{

	if ((bReceivingInput == false && bAutoSlowDown) || slowDown)
	{
		float accelCurve{(PhaseMap[GetNextPhase()] - PhaseMap[CurrentPhase]) / SlowdownSpeed};
		CurrentSpeed += (accelCurve * DeltaTime) * -1.f;
	}
	else if(bReceivingInput == true)
	{
		float accelCurve{ (PhaseMap[GetNextPhase()] - PhaseMap[CurrentPhase]) / TimeForNextPhase };
		CurrentSpeed += (accelCurve * DeltaTime);
	}

	// cap speed when not boosting
	if (bIsBoosting == false)
		CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.f, PhaseMap[EPhase::PHASE3]);
	else
		CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.f, PhaseMap[EPhase::BOOST]);
}

void AWackyNoodleCharacter::lookBehind()
{
	
	FRotator newRot{0.f,0.f,0.f};
	newRot.Yaw = 180.f;
	GetCameraBoom()->AddLocalRotation(newRot);
}

bool AWackyNoodleCharacter::CanOtherPlayerDamageThisPlayer(AWackyNoodleCharacter* otherPlayer)
{
	if (DoesTeamMatter == false)
		return true;

	return PlayerTeam != otherPlayer->PlayerTeam;
}

void AWackyNoodleCharacter::SetPlayerInfo()
{
	if (HasAuthority())
	{
		AWN_BaseGameMode* gm = Cast<AWN_BaseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		gm->setPlayerInfo(this);
	}
}


void AWackyNoodleCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWackyNoodleCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWackyNoodleCharacter::MoveForward(float Value)
{

	ServerUpdateRecieveInput(Value);
	if ((Controller != nullptr))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		float speed{ PhaseMap[CurrentPhase] };



		if (bBlendSpeed)
			speed = CurrentSpeed;

		
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, speed);
		GetCharacterMovement()->MaxWalkSpeed = PhaseMap[CurrentPhase];
	}
}


void AWackyNoodleCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr))
	{
		AddControllerYawInput(Value * BaseTurnRate * TurnRateMap[CurrentPhase] * GetWorld()->GetDeltaSeconds());
	}
}

void AWackyNoodleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWackyNoodleCharacter, CurrentPhase);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, CurrentSpeed, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bCanJump, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bIsBoosting, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bReceivingInput, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bReducePhase, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bStartDelayPhase, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter, bLowerPhase1, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWackyNoodleCharacter,slowDown, COND_OwnerOnly);
	DOREPLIFETIME(AWackyNoodleCharacter, Text_PlayerName);
	DOREPLIFETIME(AWackyNoodleCharacter, PlayerTeam);
	DOREPLIFETIME(AWackyNoodleCharacter, DoesTeamMatter);
}



void AWackyNoodleCharacter::ResetPlayerData()
{
	if (HasAuthority() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reset player data should only be called from the server"));
		return;
	}

	CurrentPhase = EPhase::PHASE0;
	CurrentSpeed = 0;
	bReceivingInput = false;
	slowDown = false;
}

void AWackyNoodleCharacter::resetDamageCooldown()
{
	bDamageCooldown = false;
}

void AWackyNoodleCharacter::LowerPlayerPhase()
{
	if (bReducePhase && !bStartDelayPhase)
	{
		if (CurrentPhase == EPhase::PHASE1 && bLowerPhase1)
		{
			CurrentPhase = EPhase::PHASE0;
			CurrentSpeed = PhaseMap[EPhase::PHASE0];
		}
		else if (CurrentPhase == EPhase::PHASE2)
		{
			CurrentPhase = EPhase::PHASE1;
			CurrentSpeed = PhaseMap[EPhase::PHASE1];
		}
		else if (CurrentPhase == EPhase::PHASE3)
		{
			CurrentPhase = EPhase::PHASE2;
			CurrentSpeed = PhaseMap[EPhase::PHASE2];

		}
		else if (CurrentPhase == EPhase::BOOST)
		{
			CurrentPhase = EPhase::PHASE3;
			CurrentSpeed = PhaseMap[EPhase::PHASE3];
		}
		
		bReducePhase = false;
		bStartDelayPhase =true;
	}

}