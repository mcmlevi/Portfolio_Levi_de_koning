// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HeatMap.h"
#include "WN_BaseGameMode.generated.h"

class AWackyNoodleCharacter;
/**
 * 
 */
UCLASS()
class WACKYNOODLE_API AWN_BaseGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AWN_BaseGameMode();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void ResetMap();
	void Tick(float DeltaSeconds) override;

	void StartMatch() override;
	virtual void setPlayerInfo(AWackyNoodleCharacter* player);


	
	void EndMatch() override;
protected:
	
	

	virtual AActor* FindRespawnLocation(APlayerController* Player);

	void VerifyRespawns();

	virtual void respawnAllPlayers();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerKilled(APlayerController* target, APlayerController* Killer);

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void KillPlayer(APlayerController* Player, APlayerController* Killer);

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	UFUNCTION(Server, Reliable)
		void resetSinglePlayer(APlayerController* Player);

	UPROPERTY()
		TArray<AActor*> respawnPoints{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MatchLenght = 10.f;

	UPROPERTY(BlueprintReadOnly)
		FTimerHandle MatchTimer;

private:
	HeatMap map;
	
};
