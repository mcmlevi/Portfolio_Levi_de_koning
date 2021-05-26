// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FGameModeNameSelectionInfo
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString GameModeName = "GameMode";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GameModeExtension = "";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString GameModeCommand = "?Game=/Path";
};


UCLASS()
class WACKYNOODLE_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
};
