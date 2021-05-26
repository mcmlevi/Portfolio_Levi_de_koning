// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WN_BaseGameMode.h"
#include "HeatMap.h"
#include "FFA_GameMode.generated.h"




class ARespawnPoint;
/**
 * 
 */
UCLASS()
class WACKYNOODLE_API AFFA_GameMode : public AWN_BaseGameMode
{
	GENERATED_BODY()
public:
	AFFA_GameMode();
protected:
	
	



	

	
	
	
	
public:
	void PostLogin(APlayerController* NewPlayer) override;

	
	void BeginPlay() override;

	





	void Tick(float DeltaSeconds) override;

private:
	
	
	
};
