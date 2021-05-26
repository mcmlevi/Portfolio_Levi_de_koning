// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UWorld;
UENUM()
enum class LogEvent
{
	POSITION,
	LEVELSIZE,
	PleayerDeath,
	RESPAWNPOINT,
	MAPNAME,

};

struct PlayerData
{
	LogEvent event = LogEvent::POSITION;
	int playerID;
	float Time;
	FVector Position;
	FVector movementDirection;
	float Velocity;
};

class WACKYNOODLE_API HeatMap
{
public:
	HeatMap();
	~HeatMap();


	TArray<PlayerData> PlayerHeatMapInfo;
	TArray<FString> PlayerHeatMapEvents;
	void SaveHeatMapData(UWorld* world);
	void addPlayerDeath(const FVector& Pos, float Time);

	private:
		void resetData();
		void addRespawnPoints(UWorld* World);
		void addMapName(UWorld* world);
		
};
