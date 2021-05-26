// Fill out your copyright notice in the Description page of Project Settings.


#include "WN_BaseGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "RespawnPoint.h"
#include  "Engine/World.h"
#include "WN_BaseGameState.h"
#include "WackyNoodle/WackyNoodleCharacter.h"
#include "Engine/Classes/GameFramework/PlayerController.h"
#include "engine/Classes/GameFramework/PawnMovementComponent.h"
#include "TimerManager.h"

AWN_BaseGameMode::AWN_BaseGameMode()
{
	// find all respawn points
	TSubclassOf<ARespawnPoint> classToFind;
	classToFind = ARespawnPoint::StaticClass();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, respawnPoints);
}

void AWN_BaseGameMode::ResetMap_Implementation()
{
	map.SaveHeatMapData(GetWorld());

	respawnAllPlayers();
	StartMatch();
}

void AWN_BaseGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AWN_BaseGameState* GS = GetGameState<AWN_BaseGameState>();

	GS->timeLeft = GetWorld()->GetTimerManager().GetTimerRemaining(MatchTimer);
	int playerCounter{};
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* pc = It->Get();
		if (pc)
		{

			PlayerData log;
			if (pc->GetPawn())
				log.Position = pc->GetPawn()->GetActorLocation();
			else
				continue;
			log.playerID = playerCounter++;


			log.Time = GetWorld()->GetTimerManager().GetTimerElapsed(MatchTimer);
			AWackyNoodleCharacter* character = Cast<AWackyNoodleCharacter>(pc->GetCharacter());
			if (character)
			{
				log.Velocity = (character->GetVelocity().Size() / character->PhaseMap[EPhase::BOOST]) * 255.f;
				log.movementDirection = character->GetMovementComponent()->Velocity;
			}



			map.PlayerHeatMapInfo.Add(log);
		}
	}
}

void AWN_BaseGameMode::StartMatch()
{
	Super::StartMatch();
	GetWorld()->GetTimerManager().SetTimer(MatchTimer, this, &AWN_BaseGameMode::EndMatch, MatchLenght);
}

void AWN_BaseGameMode::setPlayerInfo(AWackyNoodleCharacter* player)
{
	
	if (player)
	{

		AWN_BaseGameState* GS = GetGameState<AWN_BaseGameState>();
		FScore newScore{};
		newScore.PlayerName = player->Text_PlayerName;
		newScore.player = Cast<APlayerController>(player->GetController());
		FString name = player->Text_PlayerName.ToString();
		GS->Scores.Add(newScore);
	}
}

void AWN_BaseGameMode::EndMatch()
{
	Super::EndMatch();
	GetGameState<AWN_BaseGameState>()->BPEndMatch();
	ResetMap();
	bUseSeamlessTravel = false;

	FURL test = GetWorld()->GetCurrentLevel()->URL;
	GetWorld()->ServerTravel(("/Game/OtherIdkWhereToSortTo/Lobby?Game=/Game/OtherIdkWhereToSortTo/GameModes/BP_Lobby_Gamemode.BP_Lobby_Gamemode_C"));
	
}


AActor* AWN_BaseGameMode::FindRespawnLocation(APlayerController* Player)
{
	VerifyRespawns();

	AActor* BestSpawn{ nullptr };
	float distanceToClosestPlayer{ 0 };

	if (GetNumPlayers() == 0)
	{
		return respawnPoints[FMath::RandRange(0, respawnPoints.Num() - 1)];
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
			if (d < distanceToPlayer)
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

void AWN_BaseGameMode::VerifyRespawns()
{
	if (respawnPoints.Num() == 0)
	{
		UE_LOG(LogUObjectGlobals, Fatal, TEXT("Error no Respawn points in the level"));
	}
}

void AWN_BaseGameMode::respawnAllPlayers()
{
	if (HasAuthority() == false)
		return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* pc = It->Get();
		if (pc)
		{
			// Set player new location
			ARespawnPoint* rp = Cast<ARespawnPoint>(FindRespawnLocation(pc));

			pc->GetPawn()->SetActorLocation(rp->GetActorLocation());
			pc->ClientSetRotation(rp->GetActorRotation());

			// Reset player game data
			AWackyNoodleCharacter* PlayerCharacter = Cast<AWackyNoodleCharacter>(pc->GetPawn());

			if (PlayerCharacter)
			{
				PlayerCharacter->ResetPlayerData();
			}
		}
	}
}

AActor* AWN_BaseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (Player)
	{
		AActor* respawn = FindRespawnLocation(Cast<APlayerController>(Player));
		return respawn;
	}
	return nullptr;
}

void AWN_BaseGameMode::KillPlayer_Implementation(APlayerController* Player, APlayerController* Killer)
{
	OnPlayerKilled(Player, Killer);

	AWackyNoodleCharacter* pc = Cast<AWackyNoodleCharacter>(Player->GetPawn());
	if (pc->bDamageCooldown == false)
	{
		map.addPlayerDeath(Player->GetCharacter()->GetActorLocation(), GetWorld()->GetTimerManager().GetTimerElapsed(MatchTimer));
		pc->bDamageCooldown = true;
		FTimerHandle reset;
		GetWorld()->GetTimerManager().SetTimer(reset, pc, &AWackyNoodleCharacter::resetDamageCooldown, 0.2f);

		AWN_BaseGameState* gs = GetGameState<AWN_BaseGameState>();

		for (FScore& elem : gs->Scores)
		{
			if (elem.player == Killer)
			{
				elem.Points++;
				break;
			}
		}

	}


	resetSinglePlayer(Player);
}


void AWN_BaseGameMode::resetSinglePlayer_Implementation(APlayerController* Player)
{
	APlayerController* pc = Player;
	if (pc)
	{
		// Set player new location
		ARespawnPoint* rp = Cast<ARespawnPoint>(FindRespawnLocation(Player));

		pc->GetPawn()->SetActorLocation(rp->GetActorLocation());
		pc->ClientSetRotation(rp->GetActorRotation());

		// Reset player game data
		AWackyNoodleCharacter* PlayerCharacter = Cast<AWackyNoodleCharacter>(pc->GetPawn());

		if (PlayerCharacter)
		{
			PlayerCharacter->ResetPlayerData();
		}
	}
}
