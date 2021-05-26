// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WN_BaseGameMode.h"
#include "TDM_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class WACKYNOODLE_API ATDM_GameMode : public AWN_BaseGameMode
{
	GENERATED_BODY()

public:
	void setPlayerInfo(AWackyNoodleCharacter* player) override;
	
	AActor* FindRespawnLocation(APlayerController* Player) override;
	void Tick(float DeltaSeconds) override;
private:
	bool Team0 = false;
};
