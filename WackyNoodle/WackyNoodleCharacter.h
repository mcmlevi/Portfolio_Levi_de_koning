// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WackyNoodleCharacter.generated.h"

UENUM(BlueprintType)
enum class EPhase : uint8
{
	PHASE0 UMETA(DisplayName = "Phase 0"),
	PHASE1 UMETA(DisplayName = "Phase 1"),
	PHASE2 UMETA(DisplayName = "Phase 2"),
	PHASE3 UMETA(DisplayName = "Phase 3"),
	BOOST UMETA(DisplayName = "Boost")
};

UCLASS(config = Game)
class AWackyNoodleCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AWackyNoodleCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Map of The phase and desired speeds to reach them **/
	UPROPERTY(EditAnywhere, Category = "SpeedManagement", BlueprintReadOnly)
		TMap<EPhase, float> PhaseMap{
			{EPhase::PHASE0, 0.0f},
			{EPhase::PHASE1, 800.f},
			{EPhase::PHASE2, 1400.f},
			{EPhase::PHASE3, 1900.f},
			{EPhase::BOOST, 2200.f}
	};

	bool bDamageCooldown = false;
	UPROPERTY(BlueprintReadWrite, Replicated)
		FText Text_PlayerName =	FText::FromString("Player Name");

	UPROPERTY(BlueprintReadOnly, Replicated)
		int PlayerTeam = 0;
	UPROPERTY(Replicated)
		bool DoesTeamMatter = false;
	
protected:


	/** Called for forwards/backward input */
	void MoveForward(float Value);


	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void updateSpeed(float DeltaTime);

	
	void lookBehind();
	
	UFUNCTION(BlueprintCallable)
		bool CanOtherPlayerDamageThisPlayer(AWackyNoodleCharacter* otherPlayer);
	UFUNCTION(BlueprintCallable)
		void SetPlayerInfo();
protected:
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


	/** map of turnrate depending on the current phase 0 is not able to turn 1 is maximum rotation speed **/
	UPROPERTY(EditAnywhere, Category = "SpeedManagement", BlueprintReadOnly)
		TMap<EPhase, float> TurnRateMap{
			{EPhase::PHASE0, 1.f},
			{EPhase::PHASE1, 1.f},
			{EPhase::PHASE2, 0.75f},
			{EPhase::PHASE3, 0.5f},
			{EPhase::BOOST, 0.25f}
	};
	/** Time to go to next phase in seconds **/
	UPROPERTY(EditAnywhere, Category = "SpeedManagement")
	float TimeForNextPhase = 2.f;
	/** Time to go to slow down a phase in seconds **/
	UPROPERTY(EditAnywhere, Category = "SpeedManagement")
		float SlowdownSpeed = 0.5f;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	EPhase CurrentPhase = EPhase::PHASE0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float CurrentSpeed;
	
	/** Boolean if the character is in a boost state **/
	UPROPERTY(BlueprintReadWrite, replicated)
	bool bIsBoosting = false;
	/** Boolean if the character has hit a wall **/
	UPROPERTY(BlueprintReadWrite, replicated)
		bool bReducePhase = false;
	/** Boolean if the character should check if he hit wall again **/
	UPROPERTY(BlueprintReadWrite, replicated)
		bool bStartDelayPhase = false;
	UPROPERTY(BlueprintReadWrite, replicated)
		bool bLowerPhase1 = false;
	/** The delay between jumps in seconds **/
	UPROPERTY(EditAnywhere, Category = "SpeedManagement")
	float jumpDelay = 2.f;
	UPROPERTY(BlueprintReadOnly, Replicated)
		bool bCanJump = true;
	/** If we want to blend between te phases or not **/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpeedManagement")
		bool bBlendSpeed = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpeedManagement")
		bool bAutoSlowDown = false;


	
	UFUNCTION()
		void UpdatePhase();
	UFUNCTION()
		void resetJumpTimer();

	EPhase GetNextPhase();

	UPROPERTY(Replicated)
	bool bReceivingInput = false;

	
	
	public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void Jump() override;

	void ResetPlayerData();
	void resetDamageCooldown();

	void LowerPlayerPhase();
private:
	UPROPERTY(Replicated)
	bool slowDown{false};

	 
	UFUNCTION(Server,Unreliable, WithValidation)
		void ServerUpdateRecieveInput(float Value);

	

	
};

