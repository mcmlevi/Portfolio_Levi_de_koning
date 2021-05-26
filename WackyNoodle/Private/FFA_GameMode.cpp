// Fill out your copyright notice in the Description page of Project Settings.

#include "FFA_GameMode.h"

#include "Kismet/GameplayStatics.h"
#include  "Engine/World.h"
#include "RespawnPoint.h"
#include "WackyNoodle/WackyNoodleCharacter.h"
#include "Engine/Classes/GameFramework/PlayerState.h"
#include "Engine/Classes/GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Engine/Classes/GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include  "FFA_GameState.h"
#include "Misc/CoreMisc.h"
#include "Misc/FileHelper.h"
#include "engine/Classes/GameFramework/PawnMovementComponent.h"


AFFA_GameMode::AFFA_GameMode():AWN_BaseGameMode()
{
	
}




void AFFA_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	
}




void AFFA_GameMode::BeginPlay()
{
	Super::BeginPlay();
	StartMatch();
}



void AFFA_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}






