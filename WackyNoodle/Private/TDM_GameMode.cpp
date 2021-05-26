// Fill out your copyright notice in the Description page of Project Settings.


#include "TDM_GameMode.h"


#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "WackyNoodle/WackyNoodleCharacter.h"



void ATDM_GameMode::setPlayerInfo(AWackyNoodleCharacter* player)
{
	Super::setPlayerInfo(player);

	
	if (player)
	{
		player->DoesTeamMatter = true;
		player->PlayerTeam = static_cast<int>(Team0);
		Team0 = !Team0;
	}
}

AActor* ATDM_GameMode::FindRespawnLocation(APlayerController* Player)
{
	VerifyRespawns();

	AActor* BestSpawn{ respawnPoints[FMath::RandRange(0, respawnPoints.Num() - 1)] };
	AWackyNoodleCharacter* pc = Cast<AWackyNoodleCharacter>(Player->GetCharacter());
	int team = 0;
	if (pc)
		team = pc->PlayerTeam;
	
	float distanceToClosestPlayer{ 0 };

	if (GetNumPlayers() == 0)
	{
		return BestSpawn;
	}

	// loop trough all spawn points
	for (AActor*& spawnPoint : respawnPoints)
	{
		float distanceToPlayer{ INFINITY };
		// loop trough all players
		for (int i = 0; i < GetNumPlayers(); i++)
		{
			APawn* selectedPlayer = Cast<APawn>(UGameplayStatics::GetPlayerController(GetWorld(), i)->GetPawn());

			// get the closest player
			float d = selectedPlayer->GetDistanceTo(spawnPoint);
			if (d < distanceToPlayer && Cast<AWackyNoodleCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), i)->GetCharacter())->PlayerTeam != team)
				distanceToPlayer = d;
		}
		// if all players are further away the old spawn point use new best location
		if (distanceToPlayer > distanceToClosestPlayer)
		{
			distanceToClosestPlayer = distanceToPlayer;
			BestSpawn = spawnPoint;
		}
	}

	return BestSpawn;
}

void ATDM_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
