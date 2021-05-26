#include "..\Public\HeatMap.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "HeatMap.h"




#include "Hash/CityHash.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "RespawnPoint.h"
#include "LevelBounderies.h"
HeatMap::HeatMap()
{
}

HeatMap::~HeatMap()
{
}


void HeatMap::SaveHeatMapData(UWorld* world)
{

	FString data{};
	for (PlayerData& Element : PlayerHeatMapInfo)
	{
		data += UEnum::GetValueAsString(Element.event);
		data += " ";
		data += FString::FromInt(Element.playerID);
		data += " ";
		data += FString::SanitizeFloat(Element.Time);
		data += " ";
		data += (FString::SanitizeFloat(Element.Position.X) += " " + FString::SanitizeFloat(Element.Position.Y) += " " + FString::SanitizeFloat(Element.Position.Z));
		data += " ";
		data += FString::SanitizeFloat(Element.Velocity);
		data += " ";
		Element.movementDirection.Normalize();
		data += (FString::SanitizeFloat(Element.movementDirection.X) += " " + FString::SanitizeFloat(Element.movementDirection.Y) += " " + FString::SanitizeFloat(Element.movementDirection.Z));
		data += " ";
		data += "\n";
	}

	// Add Level bounds
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(world, ALevelBounderies::StaticClass(), FoundActors);

	if(FoundActors.Num() >= 1 && FoundActors[0] != nullptr)
	{
		data += UEnum::GetValueAsString(LogEvent::LEVELSIZE);
		data += " ";
		ALevelBounderies* bounds = Cast<ALevelBounderies>(FoundActors[0]);
		data += (FString::SanitizeFloat(bounds->GetActorScale().X) += " " + FString::SanitizeFloat(bounds->GetActorScale().Y) += " " + FString::SanitizeFloat(bounds->GetActorScale().Z));
		data += "\n";
		
	}
	addRespawnPoints(world);
	addMapName(world);
	for (FString& Element : PlayerHeatMapEvents)
	{
		data += Element;
	}
	
	
	// Hash current Time so we can have basically infinite files.
	FString baseDir = FPaths::ProjectDir();
	FString CurrentTime = FDateTime::Now().ToString();
	uint32 hash = CityHash32((const char*)&CurrentTime, CurrentTime.Len());
	baseDir += "\\LogData\\" + FString::FromInt(hash) + ".hmd";
	FFileHelper::SaveStringToFile(data, *baseDir);
	resetData();
}

void HeatMap::addPlayerDeath(const FVector& Pos, float Time)
{
	FString Event{};
	Event += UEnum::GetValueAsString(LogEvent::PleayerDeath);
	Event += " ";
	Event += FString::SanitizeFloat(Time);
	Event += " ";
	Event += (FString::SanitizeFloat(Pos.X) += " " + FString::SanitizeFloat(Pos.Y) += " " + FString::SanitizeFloat(Pos.Z));
	Event += "\n";
	//PlayerHeatMapEvents.Add();
	PlayerHeatMapEvents.Add(Event);
}

void HeatMap::resetData()
{
	PlayerHeatMapInfo.Empty();
	PlayerHeatMapEvents.Empty();
	
}

void HeatMap::addRespawnPoints(UWorld* world)
{
	// Add Level bounds
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass( world, ARespawnPoint::StaticClass(), FoundActors);

	FString event{};
	for (auto& Element : FoundActors)
	{
		event += UEnum::GetValueAsString(LogEvent::RESPAWNPOINT);
		event += " ";
		event += (FString::SanitizeFloat(Element->GetActorLocation().X) += " " + FString::SanitizeFloat(Element->GetActorLocation().Y) += " " + FString::SanitizeFloat(Element->GetActorLocation().Z));
		event += "\n";
	}

	PlayerHeatMapEvents.Add(event);
}

void HeatMap::addMapName(UWorld* world)
{
	FString event;
	event += UEnum::GetValueAsString(LogEvent::MAPNAME);
	event += " ";
	event += UGameplayStatics::GetCurrentLevelName(world, true);
	event += "\n";
	PlayerHeatMapEvents.Add(event);
}


