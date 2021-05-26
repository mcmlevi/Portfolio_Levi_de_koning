// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WN_BaseGameState.generated.h"
USTRUCT(BlueprintType)
struct FScore
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int Points = 0;
	UPROPERTY(BlueprintReadWrite)
		int KOs = 0;
	UPROPERTY(BlueprintReadWrite)
		int Wos = 0;
	UPROPERTY(BlueprintReadWrite)
		FText PlayerName {};

	UPROPERTY(BlueprintReadWrite)
		APlayerController* player = nullptr;

};
/**
 * 
 */
UCLASS()
class WACKYNOODLE_API AWN_BaseGameState : public AGameState
{
	GENERATED_BODY()
protected:
	UFUNCTION()
		void sortSet();

public:

	UFUNCTION(BlueprintImplementableEvent)
		void BPEndMatch();
	
	void Tick(float DeltaSeconds) override;

	UPROPERTY(Replicated, BlueprintReadOnly)
		float timeLeft;
	UPROPERTY(BlueprintReadOnly, Replicated)
		TArray<FScore> Scores{};

};
